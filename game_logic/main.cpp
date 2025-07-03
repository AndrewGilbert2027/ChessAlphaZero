#include "ChessBoard.h"
#include <iostream>
#include <vector>
#include "types.h"
#include <chrono>

int main () {
    ChessBoard *board = new ChessBoard();

    std::cout << "Initial Chess Board Setup:\n";
    size_t count = 0;
    long long total_time_us = 0;  // Use long long for microseconds
    
    while (!board->is_game_over()) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::cout << "Turn: " << count << "\n";
        Move random_move = board->random_move();
        board = board->step(random_move);
        board->print_board();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        
        total_time_us += duration.count();
        count++;
    }
    
    std::cout << "Average time per move: " << (total_time_us / count) << " microseconds\n";
    std::cout << "Number of moves per second: " << (1000000.0 / (total_time_us / count)) << " moves/s\n";
    
    return 0;
}