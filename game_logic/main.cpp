#include "ChessBoard.h"
#include <iostream>
#include <vector>
#include "types.h"

int main () {
    ChessBoard board;

    std::cout << "Initial Chess Board Setup:\n";
    board.print_board();
    std::cout << "Play a move (0,1 to 7,7):\n";
    int from_x, from_y, to_x, to_y;
    while (true) {
        std::cout << "Enter move (from_x from_y to_x to_y): ";
        std::cin >> from_x >> from_y >> to_x >> to_y;

        if (board.make_move(Move(Coords(from_x, from_y), Coords(to_x, to_y)))) {
            std::cout << "Move successful!\n";
            board.print_board();
        } else {
            std::cout << "Invalid move. Try again.\n";
        }
    }
    
    
    
    return 0;
}