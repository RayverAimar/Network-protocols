#ifndef __TIC_TAC_TOE_H__
#define __TIC_TAC_TOE_H__

#include <vector>
#include <iostream>
#include <set>

struct TicTacToe
{
    std::vector<std::vector<char>> board;
    std::vector<char> characters;
    int n;
    int _movements;

    TicTacToe(int _n) : n(_n)
    {
        for(int i = 0; i < n; i++)
        {
            std::vector<char> row;
            for(int j = 0; j < n; j++)
            {
                row.push_back(' ');
            }
            board.push_back(row);
        }
        characters.push_back('X');
        characters.push_back('O');
        _movements = 0;
    }

    void print_line(bool board)
    {
        if (board){
            std::cout << "\n ";
            for(int i = 0; i < n; i++)
            {
                std::cout << "----";
            }
            std::cout << "-";
            std::cout << "\n";
        }
        else{
            std::cout << "\n ";
            for(int i = 0; i < n; i++)
            {
                std::cout << "-----";
            }
            std::cout << "-";
            std::cout << "\n";
        }

    }

    void print_board()
    {
        for(int i = 0; i < board.size(); i++)
        {
            print_line(true);
            std::cout <<   " | ";
            for(int j = 0; j < board[i].size(); j++)
            {
                std::cout << board[i][j] << " | ";
            }
        }
        print_line(true);
    }

    void print_options()
    {
        std::cout << "\n";
        for(int i = 0; i < board.size(); i++)
        {
            print_line(false);
            std::cout <<   " | ";
            for(int j = 0; j < board[i].size(); j++)
            {
                int data = (i*n) + j + 1;
                if (data < 10) std::cout << 0;
                std::cout << data << " | ";
            }
        }
        print_line(false);
    }

    bool is_over()
    {
        for(int i = 0; i < n; i++)
        {
            std::set<char> combos;
            for(int j = 0; j < n; j++)
            {
                combos.insert(board[i][j]);
            }
            if(combos.size() == 1 && (*combos.begin()) != ' ' ) return true;
        }

        for(int i = 0; i < n; i++)
        {
            std::set<char> combos;
            for(int j = 0; j < n; j++)
            {
                combos.insert(board[j][i]);
            }
            if(combos.size() == 1 && (*combos.begin()) != ' ' ) return true;
        }

        std::set<char> main_diagonal;

        for(int i = 0; i < n; i++)
        {
            main_diagonal.insert(board[i][i]);
        }

        if(main_diagonal.size() == 1 && (*main_diagonal.begin()) != ' ' ) return true;

        std::set<char> second_diagonal;

        for(int i = 0; i < n; i++)
        {
            second_diagonal.insert(board[i][n - i - 1]);
        }

        if(second_diagonal.size() == 1 && (*second_diagonal.begin()) != ' ' ) return true;

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
        _movements++;
        movement--;
        int row = movement / n;
        int col = movement % n;
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