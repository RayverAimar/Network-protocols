#ifndef __UTILS_H__
#define __UTILS_H__

#define CONNECTION_ERROR    -1
#define TWO_DIGITS          10
#define THREE_DIGITS        100
#define FOUR_DIGITS         1000
#define FIVE_DIGITS         10000
#define MAX_NICKNAME_SIZE   100


#define BROADCAST           'B'
#define LIST                'L'
#define NICKNAME            'N'
#define CLOSE_SESSION       'R'
#define MESSAGE             'M'
#define NOTIFICATION        'N'
#define SEND_FILE           'F'
#define GAME                'G'
#define GAME_HANDLER        'H'
#define ERROR_REQUEST       'E'
#define SERVER_MESSAGE      'S'
#define NONE_ACTION         '.'
#define INVALID             '0'
#define STR_LENGTH          256

//TicTacToe protocols
#define DENY                00
#define PLAYING             10
#define ACCEPT              11
#define YOUR_TURN           12
#define INVITATION          13

#include <string>
#include <iostream>

std::string missing_zeros(int integer, int top)
{
    std::string zeroes;
    
    for(int i = integer; i < top; i*=10)
    {
        zeroes.push_back('0');
    }

    return zeroes;
}

std::string normalize_integer(int integer, int limit)
{
    std::string integer_str = std::to_string(integer);
    std::string zeroes_missing = missing_zeros(integer, limit);
    integer_str.insert(0, zeroes_missing);
    return integer_str;
}

std::string ask_for_user_nickname()
{
    std::string nickname;

    do{
        std::cout << "Enter your nickname (1-99 chars): ";
        std::cin >> nickname;
        //std::getline(std::cin, nickname);
    } while(nickname.size() >= MAX_NICKNAME_SIZE);

    return nickname;
}

std::string ask_for_receiver()
{
    
    std::string receiver_nickname;

    //char receiver_name[THREE_DIGITS];

    std::cout << "Type the receiver: ";
    
	//scanf("%[^\n]", receiver_name);

    //cin.getline(&receiver_nickname.front(), THREE_DIGITS, '\n');
    
    std::cin >> receiver_nickname;
    //getline(std::cin, receiver_nickname);

    /* do{
        std::cout << "Who do you want to send the message to? ";
        std::getline(std::cin, receiver_nickname);
    } while(receiver_nickname.size() >= MAX_NICKNAME_SIZE); */

    //string receiver_nickname(receiver_name);

    //std::cout << receiver_name << "\n";

    return receiver_nickname;
}

void waiting_response()
{
    std::cout << "\n$ ";
}



#endif //__UTILS_H__
