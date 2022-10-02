#include "TicTacToe.h"

int main()
{
    TicTacToe game;
    game.print_board();

    while(!game.is_over())
    {
        int movement;
        std::cout << "Movement: ";
        std::cin>>movement;
        game.insert_movement(movement);
        game.print_board();
    }
    std::cout << "Game is over!" << std::endl;
}