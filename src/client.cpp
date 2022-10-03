#ifdef __linux__

    #include <sys/types.h>
    #include <sys/socket.h>  
    #include <netinet/in.h>
    #include <arpa/inet.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <ctime>    
#include <chrono>
#include <vector>
#include <utility>
#include <fstream>

#include "../include/utils.h"
#include "../include/TicTacToe.h"


std::string nickname;
std::pair<bool, std::string> is_currently_playing = std::make_pair(false, ""); 
TicTacToe game(3);
int dimension_of_the_board = 3;
bool your_turn = true;

void readMessage(int SocketFD)
{
    char bufferRead[500];
    int n, size_friend;
    
    char tag = NONE_ACTION;

    while (tag != 'R')
    {
        std::string instruction;
        instruction.clear();
        n = recv(SocketFD, &tag, 1, 0);
        if(tag == LIST) //L03040606RAULRAYVERGIOMAR
        {
            std::cout << "\n";
            n = recv(SocketFD, bufferRead, 2, 0);
            int clients_online = atoi(bufferRead), cur_size;
            std::vector<int> sizes;
            sizes.push_back(0);
            int total_characters = 0;
            for(int i = 0, counter = 0; counter < clients_online; counter++, i+=2)
            {
                n = recv(SocketFD, bufferRead, 2, 0);
                bufferRead[n] = '\0';
                cur_size = atoi(bufferRead);
                sizes.push_back(cur_size);
                total_characters += cur_size;
            }
            n = recv(SocketFD, bufferRead, total_characters, 0);
            bufferRead[n] = '\0';
            for(int i = 0; i < sizes.size() - 1; i++)
            {
                std::string prefix = "   ", client_name(bufferRead, sizes[i], sizes[i + 1]);
                sizes[i + 1] += sizes[i];
                if(client_name == nickname) prefix = " * ";
                std::cout << prefix << client_name << "\n";
            }
            std::cout << "\n$ ";
        }
        else if(tag == MESSAGE)
        {
            n = recv(SocketFD, bufferRead, 5, 0);
            bufferRead[n] = '\0';
            std::string sender_nickname_size(bufferRead, 0, 2), message_size(bufferRead, 2, 3);
            int sender_size_integer = atoi(&sender_nickname_size.front());
            int message_size_integer = atoi(&message_size.front());
            n = recv(SocketFD, bufferRead, sender_size_integer+ message_size_integer, 0);
            std::string sender_nickname(bufferRead, 0 , sender_size_integer), message(bufferRead, sender_size_integer, message_size_integer);
            std::cout << "\n\t[" << sender_nickname << "] says: " << message << "\n\n$";
            //std::cout << "\n$ ";
        }
        else if(tag == NOTIFICATION)
        {
            n = recv(SocketFD, bufferRead, 3, 0);
            bufferRead[n] = '\0';
            int message_size = atoi(bufferRead);
            n = recv(SocketFD, bufferRead, message_size, 0);
            bufferRead[n] = '\0';
            std::cout << "\t\t" << bufferRead << "\n\n$";
            std::cout << "\n$ ";
        }
        else if(tag == ERROR_REQUEST)
        {
            n = recv(SocketFD, bufferRead, 3, 0);
            bufferRead[n] = '\0';
            int message_size = atoi(bufferRead);
            n = recv(SocketFD, bufferRead, message_size, 0);
            bufferRead[n] = '\0';
            std::cout << "\n\t[Server] says " << bufferRead << "\n\n$"; 
            is_currently_playing = std::make_pair(false, "");
        }
        else if(tag == SERVER_MESSAGE)
        {
            n = recv(SocketFD, bufferRead, 3, 0);
            bufferRead[n] = '\0';
            int message_size = atoi(bufferRead);
            n = recv(SocketFD, bufferRead, message_size, 0);
            std::cout << "\n\t[Server] says " << bufferRead << "\n";
            std::cout << "\n$ ";
        }
        else if(tag == GAME) //G06RAYVER0
        {
            n = recv(SocketFD, bufferRead, 2, 0);
            bufferRead[n] = '\0';
            int nickname_size = atoi(bufferRead);
            n = recv(SocketFD, bufferRead, nickname_size, 0);
            bufferRead[n]= '\0';
            std::string opponent_nickname(bufferRead);
            instruction.push_back(tag);
            instruction.append(normalize_integer(opponent_nickname.size(), TWO_DIGITS));
            instruction.append(opponent_nickname); //G06RAYVER
            n = recv(SocketFD, bufferRead, 2, 0);
            int request = atoi(bufferRead);

            if((request == INVITATION) && !is_currently_playing.first)
            {
                char answer;
                std::cout << "\t\t[" << opponent_nickname << "] invited you to play a TicTacToe match! (Press 'G' to accept!)\n";
                is_currently_playing.second = opponent_nickname;
                n = recv(SocketFD, bufferRead, 2, 0);
                dimension_of_the_board= atoi(bufferRead);
                waiting_response();
                continue;
            }
            else if((request == INVITATION) && is_currently_playing.first)
            {
                instruction.append(std::to_string(PLAYING));
                //std::cout << instruction << "\n";
                n = send(SocketFD, &instruction.front(), instruction.size(), 0);

            }
            else if(request == DENY)
            {
                std::cout << "\t\t[" << opponent_nickname << "] denied the match :(\n";
                is_currently_playing = std::make_pair(false, "");
            }
            else if(request == PLAYING)
            {
                std::cout << "\t\t[" << opponent_nickname << "] is currently playing another match :(\n";
                is_currently_playing = std::make_pair(false, "");
            }
            else if(request == ACCEPT)
            {
                std::cout << "\t\t[" << opponent_nickname << "] accepted the match :)\n";
            }
            else
            {
                game.insert_movement(request); //Push movement
                if(game.is_over()){
                    game.print_board();
                    game.clear();
                    is_currently_playing = std::make_pair(false, "");
                    std::cout << "\t\t [" << opponent_nickname << "] won the match!\n$ ";
                    continue;
                }
                if(game.is_tied())
                {
                    game.print_board();
                    game.clear();
                    is_currently_playing = std::make_pair(false, "");
                    std::cout << "\t\t Draw!\n$ ";
                    continue;
                }
                your_turn = true;
                std::cout << "\t\t[" << opponent_nickname << "] made a new movement!\n";
                game.print_board();
            }
            std::cout << "\n$ ";
        }
        else if(tag == GAME_HANDLER)
        {
            std::cout << "\n$ ";
        }
        else if (tag == BROADCAST) //B04006RAULHOLAAA
        {
            n = recv(SocketFD, bufferRead, 2, 0);
            bufferRead[n] = '\0';
            int sender_nickname_size = atoi(bufferRead);
            n = recv(SocketFD, bufferRead, 3, 0);
            bufferRead[n] = '\0';
            int message_size = atoi(bufferRead);
            n = recv(SocketFD, bufferRead, sender_nickname_size, 0);
            bufferRead[n] = '\0';
            std::string sender_nickname(bufferRead);
            n = recv(SocketFD, bufferRead, message_size, 0);
            bufferRead[n] = '\0';
            std::string message(bufferRead);
            std::cout << instruction << "\n";
            std::cout << "\t\t[" << sender_nickname << "] shouted: " << message << "\n\n$";
        }
        else if(tag == SEND_FILE)
        {
            int bytes_per_packet = 100;
            int size_file, size_filename;
            n = recv(SocketFD, bufferRead, 5, 0);
            bufferRead[n] = '\0';
            std::string size_filename_str(bufferRead, 0, 2),
                size_file_str(bufferRead, 2, 3);

            size_filename = atoi(&size_filename_str.front());
            size_file = atoi(&size_file_str.front());

            n = recv(SocketFD, bufferRead, size_filename, 0);
            bufferRead[n] = '\0';
            std::string filename(bufferRead, 0, size_filename);
            std::ofstream outfile;
            std::string path = "receiver_folder/" + filename;
            outfile.open(path, std::ios::out);
            while (size_file > 0)
            {
                n = recv(SocketFD, bufferRead, bytes_per_packet, 0);
                bufferRead[n] = '\0';
                outfile << bufferRead;

                size_file -= bytes_per_packet;

                if (size_file < bytes_per_packet)
                    bytes_per_packet = size_file;
            }

            outfile.close();

            std::cout << "You have got a new file. Go see it!\n";

            //n = send(SocketFD, "U", 1, 0);
            
            //std::string response = "  🔑 file downloaded successfully ";
            //std::string response_size_str = normalize_integer(response.size(), THREE_DIGITS);

            //n = send(SocketFD, &(response_size_str.front()), 3, 0);
            //n = send(SocketFD, &(response.front()), response.size(), 0);
        }
        else{
            std::cout << "\n\t[NO OPTION RECOGNIZED]\n$";
        }       
    }
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
}

std::string ask_for_message()
{
    std::string message;
    std::cout << "Type your message: \n";
    //cin.getline(&message.front(), THREE_DIGITS, '\n');
    std::cin >> message;
    //getline(std::cin, message);
    return message;
}

int main(void)
{
    char buffer[256];
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
    if (SocketFD == CONNECTION_ERROR)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(45123);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
    if (0 > Res)
    {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);  
        exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
        perror("char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    if (connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)) == CONNECTION_ERROR)
    {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    
    std::string instruction;
    char option = NONE_ACTION;

    instruction.clear();
    nickname = ask_for_user_nickname();
    instruction.push_back(NICKNAME);
    instruction.append(normalize_integer(nickname.size(), TWO_DIGITS));
    instruction.append(nickname);

    //std::cout << instruction << "\n";

    n = send(SocketFD, &instruction.front(), instruction.size(), 0);
    
    std::thread (readMessage, SocketFD).detach();

    waiting_response();

    while(option != CLOSE_SESSION){
        instruction.clear();
        std::cin >> option;
        if(option == MESSAGE)
        {
            instruction.clear();
            std::string receiver_nickname, message;
            receiver_nickname = ask_for_receiver();
            message = ask_for_message();
            instruction.push_back(MESSAGE);
            instruction.append(normalize_integer(receiver_nickname.size(), TWO_DIGITS));
            instruction.append(normalize_integer(message.size(), THREE_DIGITS));
            instruction.append(receiver_nickname);
            instruction.append(message);

            //std::cout << instruction << "\n";

            n = send(SocketFD, &instruction.front(), instruction.size(), 0 );
            std::cout << "\n$ ";            
        }
        else if (option == LIST)
        {
            instruction.clear();
            instruction.push_back(LIST);
            n = send(SocketFD, &instruction.front(), instruction.size(), 0);
            std::cout << "\n$ ";
        }
        else if(option == BROADCAST)
        {
            instruction.clear();
            std::string message;
            std::cout << "Message for all: ";
            std::cin >> message;
            instruction.push_back(BROADCAST);
            instruction.append(normalize_integer(message.size(), THREE_DIGITS));
            instruction.append(message);
            std::cout << instruction << "\n";
            n = send(SocketFD, &instruction.front(), instruction.size(), 0);
            std::cout << "$ ";
        }
        else if(option == CLOSE_SESSION)
        {
            instruction.clear();
            n = send(SocketFD, &option, 1, 0);
        }
        else if(option == NICKNAME)
        {
            instruction.clear();
            nickname = ask_for_user_nickname();
            instruction.push_back(NICKNAME);
            instruction.append(normalize_integer(nickname.size(), TWO_DIGITS));
            instruction.append(nickname);
            n = send(SocketFD, &instruction.front(), instruction.size(), 0);
            std::cout << "\n$ ";
            //N06RAYVER
        }
        else if(option == SEND_FILE)
        {
            std::string file_name;
            std::ifstream afile;
            std::string nick_friend;
            int size_file;

            int bytes_per_packet = 100;

            std::cout << "Enter your friend's nickname: ";
            std::cin >> nick_friend;
            std::cout << "Type file name: ";
            std::cin >> file_name;
            afile.open(file_name, std::ios::in);
            afile.seekg(0, afile.end);
            size_file = afile.tellg();
            afile.seekg(0, afile.beg);
            n = send(SocketFD, &option, 1, 0);
            std::string block = normalize_integer(nick_friend.size(), TWO_DIGITS) +
                    normalize_integer(file_name.size(), TWO_DIGITS) +
                    normalize_integer(size_file, THREE_DIGITS);

            n = send(SocketFD, block.c_str(), 7, 0);

            block.clear();

            block = nick_friend + file_name;

            n = send(SocketFD, &(block.front()), block.size(), 0);

            block.clear();

            char buffer[STR_LENGTH];
            while (size_file > 0)
            {
                afile.read(buffer, bytes_per_packet);

                n = send(SocketFD, buffer, bytes_per_packet, 0);

                size_file -= bytes_per_packet;

                if (size_file < bytes_per_packet)
                    bytes_per_packet = size_file;
            }

            afile.close();

        }
        else if(option == GAME) // G06RAYVER1
        {
            //instruction.push_back(GAME);
            if(!is_currently_playing.first)
            {
                if(is_currently_playing.second != "")
                {
                    instruction.clear();
                    instruction.push_back(GAME);
                    instruction.append(normalize_integer(is_currently_playing.second.size(), TWO_DIGITS));
                    instruction.append(is_currently_playing.second);
                    
                    /* char response = NONE_ACTION;
                    std::cout << "Do you want to play with [" << is_currently_playing.second <<"] (y/n)\n$";
                    std::cin >> response;

                    std::cout << "You typed -> " << response << "\n"; 
                    if(response == 'y')
                    {
                        is_currently_playing.first = true;
                        instruction.append(normalize_integer(ACCEPT, TWO_DIGITS));
                    }
                    else
                    {
                        is_currently_playing = std::make_pair(false, "");
                        instruction.append(normalize_integer(DENY, TWO_DIGITS));
                    }
                    //std::cout << instruction << "\n"; */
                    instruction.append(normalize_integer(ACCEPT, TWO_DIGITS));
                    game = TicTacToe(dimension_of_the_board);
                    //std::cout << instruction << "-" << instruction.size() << "..." <<"\n";
                    n = send(SocketFD, &instruction.front(), instruction.size(), 0);
                    is_currently_playing.first = true;
                    continue;
                }
                else
                {
                    std::string opponent_nickname;
                    std::cout << "Type who you want to play with: ";
                    std::cin >> opponent_nickname;
                    int dimension;
                    std::cout << "Type the dimension of the board: ";
                    std::cin >> dimension;
                    game = TicTacToe(dimension);
                    is_currently_playing = std::make_pair(true, opponent_nickname);
                    instruction.clear();
                    instruction.push_back(GAME);
                    instruction.append(normalize_integer(opponent_nickname.size(), TWO_DIGITS));
                    instruction.append(opponent_nickname);
                    instruction.append(normalize_integer(INVITATION, TWO_DIGITS));
                    instruction.append(normalize_integer(dimension, TWO_DIGITS));
                    n = send(SocketFD, &instruction.front(), instruction.size(), 0);
                }
            }
            else
            {
                if(your_turn)
                {
                    if(game._movements == 0)
                    {
                        game.print_board();
                    }
                    game.print_options();
                    int movement = 0;
                    std::cout << "Type your movement: ";
                    std::cin >> movement;
                    game.insert_movement(movement);
                    instruction.clear();
                    instruction.push_back(GAME);
                    instruction.append(normalize_integer(is_currently_playing.second.size(), TWO_DIGITS));
                    instruction.append(is_currently_playing.second);
                    instruction.append(normalize_integer(movement, TWO_DIGITS));
                    n = send(SocketFD, &instruction.front(), instruction.size(), 0);
                    if(game.is_over())
                    {
                        game.print_board();
                        game.clear();
                        std::cout << "You won! :) ";
                        is_currently_playing = std::make_pair(false, "");
                    }
                    your_turn = false;
                    std::cout << "\n$ ";
                }
                else{
                    std::cout << "[" << is_currently_playing.second << "] hasn't already made a movement :( \n$ ";
                }

            }
        }

        else
        {
            std::cout << "[NO OPTION ALLOWED]\n\n$ ";
        }

    };

    std::cout << "\t\tYou left the chat.\n";

    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
}
