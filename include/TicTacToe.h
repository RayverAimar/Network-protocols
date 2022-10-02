#ifndef __TIC_TAC_TOE_H__
#define __TIC_TAC_TOE_H__

#include <vector>
#include <iostream>

struct TicTacToe
{
    std::vector<std::vector<char>> board;
    std::vector<char> characters;

    TicTacToe()
    {
        for(int i = 0; i < 3; i++)
        {
            std::vector<char> row;
            for(int j = 0; j < 3; j++)
            {
                row.push_back(' ');
            }
            board.push_back(row);
        }
        characters.push_back('X');
        characters.push_back('O');
    }

    void print_board()
    {
        for(int i = 0; i < board.size(); i++)
        {
            std::cout << "\n -------------\n";
            std::cout <<   " | ";
            for(int j = 0; j < board[i].size(); j++)
            {
                std::cout << board[i][j] << " | ";
            }
        }
        std::cout<<"\n -------------\n";
    }

    void print_options()
    {
        std::cout << "\n";
        for(int i = 0; i < board.size(); i++)
        {
            std::cout << "\n -------------\n";
            std::cout <<   " | ";
            for(int j = 0; j < board[i].size(); j++)
            {
                std::cout << (i*3) + j + 1 << " | ";
            }
        }
        std::cout<<"\n -------------\n";
    }

    bool is_over()
    {
        for(int i = 0; i < characters.size(); i++)
        {
            if(board[0][0] == characters[i] && board[0][1] == characters[i] && board[0][2] == characters[i]) return true;
            if(board[1][0] == characters[i] && board[1][1] == characters[i] && board[1][2] == characters[i]) return true;
            if(board[2][0] == characters[i] && board[2][1] == characters[i] && board[2][2] == characters[i]) return true;

            if(board[0][0] == characters[i] && board[1][0] == characters[i] && board[2][0] == characters[i]) return true;
            if(board[0][1] == characters[i] && board[1][1] == characters[i] && board[2][1] == characters[i]) return true;
            if(board[0][2] == characters[i] && board[1][2] == characters[i] && board[2][2] == characters[i]) return true;

            if(board[0][0] == characters[i] && board[1][1] == characters[i] && board[2][2] == characters[i]) return true;
            if(board[0][2] == characters[i] && board[1][1] == characters[i] && board[2][0] == characters[i]) return true;
        }

        return false;
    }

    bool is_tied()
    {
        for(int i = 0; i < board.size(); i++)
        {
            for(int j = 0; j < board[i].size(); j++)
            {
                if(board[i][j] == ' ') return false;
            }
        }
        return true;
    }


    void insert_movement(int movement)
    {
        movement--;
        int row = movement / 3;
        int col = movement % 3;
        //std::cout << "movement: ("<<row<<", "<<col<<")\n";
        int movements_counter = 0;
        for(int i = 0; i < board.size(); i++)
        {
            for(int j = 0; j < board[i].size(); j++)
            {
               if(board[i][j] != ' ') movements_counter++;
            }
        }
        char cur_character = characters[movements_counter % 2];
        board[row][col] = cur_character;
    }

    void clear()
    {
        for(int i = 0; i < board.size(); i++)
        {
            for(int j = 0; j < board[i].size(); j++)
            {
               board[i][j] = ' ';
            }
        }
    }
};



#endif //__TIC_TAC_TOE_H__