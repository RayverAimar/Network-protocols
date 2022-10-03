
#ifdef __linux__

    #include <sys/types.h>
    #include <sys/socket.h>  
    #include <netinet/in.h>
    #include <arpa/inet.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <thread>
#include <map>
#include <stdlib.h>
#include <time.h>

#include "../include/utils.h"
#include "../include/client.h"
#include "../include/TicTacToe.h"


std::map<int, Client> clients;

std::map<int, std::string> names;
std::map<int, bool> is_currently_playing;

void readClient(int cur_client_socket)
{
    
    char option = NONE_ACTION;
    int n;
    char client_buffer[FOUR_DIGITS];

    while (option != CLOSE_SESSION)
    {
        n = recv(cur_client_socket, &option, 1, 0);

        std::string instruction;

        if(option == MESSAGE) //     |M|04|06|RAUL|HOLAAA|
        {
            instruction.push_back(option);

            n = recv(cur_client_socket, client_buffer, 5, 0 );
            client_buffer[n] = '\0';
            std::string receiver_nickname_size(client_buffer, 0, 2), message_size(client_buffer, 2, 3); 
            n = recv(cur_client_socket, client_buffer, atoi(&receiver_nickname_size.front()) + atoi(&message_size.front()), 0);
            client_buffer[n] = '\0';
            std::string receiver_nickname(client_buffer, 0, atoi(&receiver_nickname_size.front())), message(client_buffer, atoi(&receiver_nickname_size.front()), atoi(&message_size.front()));
            bool found = false;
            int receiver_socket = -1;
            for(auto it = names.begin(); it != names.end(); ++it)
            {
                if(it->second == receiver_nickname)
                {
                    receiver_socket = it->first;
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                std::string error_message = "Error: User '" + receiver_nickname  + "' Not Found";
                instruction.clear();
                instruction.push_back(ERROR_REQUEST);
                instruction.append(normalize_integer(error_message.size(), THREE_DIGITS));
                instruction.append(error_message);
                n = send(cur_client_socket, &instruction.front(), instruction.size(), 0);
                std::cout << "ERROR:\t\t User ["<< names[cur_client_socket] << "] requested an invalid user.\n";
                continue;
            }

            instruction.append(normalize_integer(names[cur_client_socket].size(), TWO_DIGITS));
            instruction.append(normalize_integer(message.size(), THREE_DIGITS));
            instruction.append(names[cur_client_socket]);
            instruction.append(message);

            n = send(receiver_socket, &instruction.front(), instruction.size(), 0);

            std::cout << "MESSAGE:\t\t [" << names[cur_client_socket] << "] sent message to [" << receiver_nickname << "].\n";
            std::cout << "\t\t "<< option << receiver_nickname_size << message_size << receiver_nickname << message << "\n"; 
        }
        else if (option == LIST)
        {
            instruction.push_back(option);
            instruction.append(normalize_integer(names.size(), TWO_DIGITS));

            for(auto it = names.begin(); it != names.end(); ++it)
            {
                instruction.append(normalize_integer(it->second.size(), TWO_DIGITS));
            }

            for(auto it = names.begin(); it != names.end(); ++it)
            {
                instruction.append(it->second);
            }

            //std::cout << instruction << "\n";

            n = send(cur_client_socket, &instruction.front(), instruction.size(), 0);

            std::cout << "LIST:\t\t User list requested by [" << names[cur_client_socket] << "].\n";
        }
        else if(option == BROADCAST)
        {
            n = recv(cur_client_socket, client_buffer, 3, 0);
            client_buffer[n] = '\0';
            int message_size = atoi(client_buffer);
            n = recv(cur_client_socket, client_buffer, message_size, 0);
            client_buffer[n] = '\0';
            std::string message(client_buffer);
            instruction.push_back(BROADCAST);
            instruction.append(normalize_integer(names[cur_client_socket].size(), TWO_DIGITS));
            instruction.append(normalize_integer(message_size, THREE_DIGITS));
            instruction.append(names[cur_client_socket]);
            instruction.append(message);
            for(auto it = names.begin(); it != names.end(); ++it)
            {
                if(it->second == names[cur_client_socket]) continue;
                n = send(it->first, &instruction.front(), instruction.size(), 0);
            }

            std::cout << "BROADCAST:\t User [" << names[cur_client_socket] <<"] broadcasted a message.\n";
        }
        else if(option == CLOSE_SESSION)
        {
            std::string message = names[cur_client_socket] + " just left the chat.";

            instruction.push_back(NOTIFICATION);
            instruction.append(normalize_integer(message.size(), THREE_DIGITS));
            instruction.append(message);
            std::cout << "EXIT:\t\t User [" <<names[cur_client_socket] << "] logged off.\n";
            names.erase(cur_client_socket); // Deleting client from database

            for(auto it = names.begin(); it != names.end(); ++it)
            {
                n = send(it->first, &instruction.front(), instruction.size(), 0);
                std::cout << "\t\t\t Sending notification to [" << it->second << "]"<< ".\n";
            }
        }
        else if(option == NICKNAME)
        {
            n = recv(cur_client_socket, client_buffer, 2, 0);
            client_buffer[n] = '\0';
            int new_nickname_size = atoi(client_buffer);
            n = recv(cur_client_socket, client_buffer, new_nickname_size, 0);
            client_buffer[n] = '\0';
            std::string new_nickname(client_buffer), old_nickname(names[cur_client_socket]);
            names[cur_client_socket] = new_nickname;
            
            std::string server_message = "Nickname succesfully updated from " + old_nickname + " to " + new_nickname;

            instruction.push_back(SERVER_MESSAGE);
            instruction.append(normalize_integer(server_message.size(), THREE_DIGITS));
            instruction.append(server_message);

            n = send(cur_client_socket, &instruction.front(), instruction.size(), 0);

            std::cout << "NICKNAME: \t\t User ["<< old_nickname <<"] requested a nickname update to [" << new_nickname << "].\n";
        }
        else if(option == SEND_FILE)
        {
            int size_filename, size_file, bytes_per_packet = 100;

            n = recv(cur_client_socket, client_buffer, 7, 0);
            client_buffer[n] = '\0';
            std::string size_friend_nick_str(client_buffer, 0, 2),
                size_filename_str(client_buffer, 2, 2),
                size_file_str(client_buffer, 4, 3);

            int size_friend_nick = atoi(&size_friend_nick_str.front());
            size_filename = atoi(&size_filename_str.front());
            size_file = atoi(&size_file_str.front());

            n = recv(cur_client_socket, client_buffer, size_friend_nick + size_filename, 0);
            client_buffer[n] = '\0';

            std::string nickname_friend(client_buffer, 0, size_friend_nick),
                filename(client_buffer, size_friend_nick, size_filename);

            int i;
            int sd_friend = -1;
            for (auto it = names.begin(); it != names.end() && sd_friend == -1; ++it)
            {
                if (nickname_friend == it->second)
                {
                    sd_friend = it->first;
                }
            }

            n = send(sd_friend, &option, 1, 0);
            std::string block = size_filename_str + size_file_str;
            n = send(sd_friend, &(block.front()), block.size(), 0);
            n = send(sd_friend, &(filename.front()), filename.size(), 0);
            while (size_file > 0)
            {
                n = recv(cur_client_socket, client_buffer, bytes_per_packet, 0);
                client_buffer[n] = '\0';

                n = send(sd_friend, client_buffer, bytes_per_packet, 0);

                size_file -= bytes_per_packet;

                if (size_file < bytes_per_packet)
                    bytes_per_packet = size_file;
            }

            block.clear();
        }
        else if(option == GAME)
        {
            n = recv(cur_client_socket, client_buffer, 2, 0);
            client_buffer[n] = '\0';
            int opponent_nickname_size = atoi(client_buffer);
            n = recv(cur_client_socket, client_buffer, opponent_nickname_size, 0);
            client_buffer[n] = '\0';
            std::string opponent_nickname(client_buffer);
            bool found = false;
            int receiver_socket = -1;
            for(auto it = names.begin(); it != names.end(); ++it)
            {
                if(it->second == opponent_nickname)
                {
                    receiver_socket = it->first;
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                std::string error_message = "Error: User '" + opponent_nickname  + "' Not Found";
                instruction.clear();
                instruction.push_back(ERROR_REQUEST);
                instruction.append(normalize_integer(error_message.size(), THREE_DIGITS));
                instruction.append(error_message);
                n = send(cur_client_socket, &instruction.front(), instruction.size(), 0);
                continue;
            }
            instruction.push_back(GAME);
            instruction.append(normalize_integer(names[cur_client_socket].size(), TWO_DIGITS));
            instruction.append(names[cur_client_socket]);
            n = recv(cur_client_socket, client_buffer, 2, 0);
            client_buffer[n] = '\0';
            int request = atoi(client_buffer);
            instruction.append(normalize_integer(request, TWO_DIGITS));
            n = send(receiver_socket, &instruction.front(), instruction.size(), 0);
            std::cout << "GAME:\t\t\t User [" << names[cur_client_socket] << "] coordinating a match with [" << opponent_nickname << "].\n";
        }
        else
        {
            //std::cout << "NONEACTION:\t\t No specified protocol.\n";
        }
    }
}

int main()
{
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP), n;
    char client_buffer[256];
    struct sockaddr_in stCliAddr;
    socklen_t cur_client;
    if(SocketFD == CONNECTION_ERROR)
    {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(SocketFD, SOL_SOCKET, SO_REUSEADDR, "1", sizeof(int)) == CONNECTION_ERROR)
    {
        perror("can not reuse local addresses");
        exit(1);
    }
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(45123);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
    if(bind(SocketFD,(struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)) == CONNECTION_ERROR)
    {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    if(listen(SocketFD, 10) == CONNECTION_ERROR)
    {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    while(true)
    {
        cur_client = sizeof(struct sockaddr_in);
        int client_id = accept(SocketFD, (struct sockaddr *)&stCliAddr, &cur_client);        
        n = recv(client_id, client_buffer, 1, 0);
        char option = client_buffer[0];
        n = recv(client_id, client_buffer, 2, 0);
        client_buffer[n] = '\0';
        int size_of_client_nickname = atoi(client_buffer);
        n = recv(client_id, client_buffer, size_of_client_nickname, 0);
        client_buffer[n] = '\0';
        std::string nickname(client_buffer);
        std::cout << "NEW CLIENT:\t\t [" << nickname << "]\n";
        
        Client temporal_client(nickname);
        clients[client_id] = temporal_client;
        names[client_id] = nickname;

        std::cout << clients[client_id].name << " | " << clients[client_id].is_playing << "\n";

        std::thread(readClient, client_id).detach();
    }

    close(SocketFD);

    return 0;

}