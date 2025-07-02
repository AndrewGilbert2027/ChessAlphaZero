#include "ChessBoard.h"
#include <iostream>
#include <vector>
#include "types.h"

int main () {
    ChessBoard board;

    std::cout << "Initial Chess Board Setup:\n";
    board.print_board();
    board.make_move(Move(Coords(1, 0), Coords(3, 0))); // Move a pawn
    board.print_board();
    board.make_move(Move(Coords(6, 1), Coords(4, 1))); // Move a black pawn
    board.print_board();
    board.make_move(Move(Coords(3, 0), Coords(4, 1))); // Capture a black pawn
    
    
    
    return 0;
}