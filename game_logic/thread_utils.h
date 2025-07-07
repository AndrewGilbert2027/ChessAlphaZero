#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H

/**
 * At the moment, this file contains utilities for parallelizing the initialization, 
 * copying, and evaluation of the chess board. 
 * 
 * In the end, these utilities were not used in the final implementation. (Thrashing caused performance slow down)
 */

#include "ChessBoard.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include <pthread.h>
#include <vector>
#include <atomic>

// Forward declaration of the main piece copying utility
static Piece* copy_piece(const Piece* piece);

/*
================================================================================
 UTILITIES FOR PARALLEL BOARD INITIALIZATION AND COPYING
================================================================================
*/

/**
 * @struct CopyRowData
 * @brief Data structure to pass information to a thread for copying a single row of the board.
 */
struct CopyRowData {
    int row;
    const ChessBoard* from_board;
    ChessBoard* to_board;
    CopyRowData(int r, const ChessBoard* from, ChessBoard* to) : row(r), from_board(from), to_board(to) {}
};

/**
 * @brief Thread function to copy a single row from one board to another.
 * @param arg A void pointer to a CopyRowData struct.
 */
void* copy_row(void* arg) {
    CopyRowData* data = static_cast<CopyRowData*>(arg);
    for (int col = 0; col < 8; ++col) {
        const Piece* piece = data->from_board->_board[data->row][col];
        data->to_board->_board[data->row][col] = piece ? copy_piece(piece) : nullptr;
    }
    delete data;
    return nullptr;
}

/**
 * @struct InitRowData
 * @brief Data structure to pass information to a thread for initializing a single row of the board.
 */
struct InitRowData {
    int row;
    ChessBoard* board;
    InitRowData(int r, ChessBoard* b) : row(r), board(b) {}
};

/**
 * @brief Thread function to initialize a single row of the board to its starting position.
 * @param arg A void pointer to an InitRowData struct.
 */
void* init_row(void* arg) {
    InitRowData* data = static_cast<InitRowData*>(arg);
    int row = data->row;
    ChessBoard* board = data->board;

    switch(row) {
        case 0: // White major pieces
            board->_board[0][0] = new Rook(Color::WHITE); board->_board[0][1] = new Knight(Color::WHITE);
            board->_board[0][2] = new Bishop(Color::WHITE); board->_board[0][3] = new Queen(Color::WHITE);
            board->_board[0][4] = new King(Color::WHITE); board->_board[0][5] = new Bishop(Color::WHITE);
            board->_board[0][6] = new Knight(Color::WHITE); board->_board[0][7] = new Rook(Color::WHITE);
            break;
        case 1: for (int i = 0; i < 8; ++i) board->_board[1][i] = new Pawn(Color::WHITE); break;
        case 6: for (int i = 0; i < 8; ++i) board->_board[6][i] = new Pawn(Color::BLACK); break;
        case 7: // Black major pieces
            board->_board[7][0] = new Rook(Color::BLACK); board->_board[7][1] = new Knight(Color::BLACK);
            board->_board[7][2] = new Bishop(Color::BLACK); board->_board[7][3] = new Queen(Color::BLACK);
            board->_board[7][4] = new King(Color::BLACK); board->_board[7][5] = new Bishop(Color::BLACK);
            board->_board[7][6] = new Knight(Color::BLACK); board->_board[7][7] = new Rook(Color::BLACK);
            break;
        default: for (int j = 0; j < 8; ++j) board->_board[row][j] = nullptr; break;
    }
    delete data;
    return nullptr;
}


/*
================================================================================
 UTILITIES FOR PARALLEL SEARCH (e.g., Alpha-Beta)
================================================================================
*/

/**
 * @struct SearchData
 * @brief Data structure for passing information to a search thread.
 * This is useful for parallelizing the search of different moves at the root.
 */
struct SearchData {
    ChessBoard* board;
    Move move;
    int depth;
    int* best_score; // Pointer to a shared best score
    Move* best_move; // Pointer to a shared best move
    pthread_mutex_t* mutex; // Mutex to protect shared variables
};

/**
 * @brief A thread function that would perform a search for a single root move.
 * @param arg A void pointer to a SearchData struct.
 */
void* search_move_thread(void* arg) {
    // SearchData* data = static_cast<SearchData*>(arg);
    // ChessBoard* next_board = data->board->step(data->move);
    // if (next_board) {
    //     // The alpha/beta values would need to be managed carefully here.
    //     int score = -negamax(*next_board, data->depth - 1, -BETA, -ALPHA);
    //     delete next_board;
    //
    //     pthread_mutex_lock(data->mutex);
    //     if (score > *data->best_score) {
    //         *data->best_score = score;
    //         *data->best_move = data->move;
    //     }
    //     pthread_mutex_unlock(data->mutex);
    // }
    // delete data;
    return nullptr;
}


/*
================================================================================
 UTILITIES FOR PARALLEL EVALUATION
================================================================================
*/

/**
 * @struct EvaluationData
 * @brief Data structure for parallelizing the board evaluation.
 */
struct EvaluationData {
    const ChessBoard* board;
    int start_row;
    int end_row;
    std::atomic<int>* total_score; // Atomic integer for thread-safe score accumulation
};

/**
 * @brief A thread function to evaluate a subset of the board's rows.
 * @param arg A void pointer to an EvaluationData struct.
 */
void* evaluate_rows_thread(void* arg) {
    // EvaluationData* data = static_cast<EvaluationData*>(arg);
    // int score = 0;
    // for (int i = data->start_row; i < data->end_row; ++i) {
    //     for (int j = 0; j < 8; ++j) {
    //         Piece* p = data->board->_board[i][j];
    //         if (p) {
    //             int piece_value = get_piece_value(p); // Assuming get_piece_value exists
    //             score += (p->get_color() == Color::WHITE) ? piece_value : -piece_value;
    //         }
    //     }
    // }
    // data->total_score->fetch_add(score);
    // delete data;
    return nullptr;
}


/**
 * @brief Helper function to create a deep copy of a piece.
 * This is essential for creating new board states without modifying existing ones.
 * @param piece The piece to copy.
 * @return A pointer to a new Piece object.
 */
static Piece* copy_piece(const Piece* piece) {
    if (!piece) return nullptr;
    switch (piece->get_type()) {
        case 'p': return new Pawn(*static_cast<const Pawn*>(piece));
        case 'r': return new Rook(*static_cast<const Rook*>(piece));
        case 'n': return new Knight(*static_cast<const Knight*>(piece));
        case 'b': return new Bishop(*static_cast<const Bishop*>(piece));
        case 'q': return new Queen(*static_cast<const Queen*>(piece));
        case 'k': return new King(*static_cast<const King*>(piece));
        default: return nullptr;
    }
}

#endif // THREAD_UTILS_H