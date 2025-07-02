#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H
#include "Piece.h"
#include "types.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <map>


// Function to copy a piece based on its type
Piece* copy_piece(const Piece* piece) {
    if (dynamic_cast<const Pawn*>(piece)) {
        return new Pawn(*dynamic_cast<const Pawn*>(piece));
    } else if (dynamic_cast<const Rook*>(piece)) {
        return new Rook(*dynamic_cast<const Rook*>(piece));
    } else if (dynamic_cast<const Knight*>(piece)) {
        return new Knight(*dynamic_cast<const Knight*>(piece));
    } else if (dynamic_cast<const Bishop*>(piece)) {
        return new Bishop(*dynamic_cast<const Bishop*>(piece));
    } else if (dynamic_cast<const Queen*>(piece)) {
        return new Queen(*dynamic_cast<const Queen*>(piece));
    } else if (dynamic_cast<const King*>(piece)) {
        return new King(*dynamic_cast<const King*>(piece));
    }
    return nullptr; // Should not happen
}

class ChessBoard {
private:
    Piece* _board[8][8]; // 8x8 board of pieces
    Color _turn; // Current turn (WHITE or BLACK)
public:
    // Constructor to set up the board
    ChessBoard() {
        _turn = Color::WHITE;

        // Place White pieces
        _board[0][0] = new Rook(Color::WHITE);
        _board[0][1] = new Knight(Color::WHITE);
        _board[0][2] = new Bishop(Color::WHITE);
        _board[0][3] = new Queen(Color::WHITE);
        _board[0][4] = new King(Color::WHITE);
        _board[0][5] = new Bishop(Color::WHITE);
        _board[0][6] = new Knight(Color::WHITE);
        _board[0][7] = new Rook(Color::WHITE);
        for (int i = 0; i < 8; ++i) {
            _board[1][i] = new Pawn(Color::WHITE);
        }

        // Initialize empty squares
        for (int i = 2; i < 6; ++i) {
            for (int j = 0; j < 8; ++j) {
                _board[i][j] = nullptr;
            }
        }

        // Place Black pieces
        for (int i = 0; i < 8; ++i) {
            _board[6][i] = new Pawn(Color::BLACK);
        }
        _board[7][0] = new Rook(Color::BLACK);
        _board[7][1] = new Knight(Color::BLACK);
        _board[7][2] = new Bishop(Color::BLACK);
        _board[7][3] = new Queen(Color::BLACK);
        _board[7][4] = new King(Color::BLACK);
        _board[7][5] = new Bishop(Color::BLACK);
        _board[7][6] = new Knight(Color::BLACK);
        _board[7][7] = new Rook(Color::BLACK);

        this->safe_squares = get_safe_squares(); // Initialize safe squares
    }

    // Destructor to clean up memory
    ~ChessBoard() {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                delete _board[i][j];
            }
        }
    }

    // Copy constructor
    ChessBoard(const ChessBoard& other) {
        _turn = other._turn;
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (other._board[i][j] != nullptr) {
                    _board[i][j] = copy_piece(other._board[i][j]);
                } else {
                    _board[i][j] = nullptr;
                }
            }
        }
    }

    // Assignment operator
    ChessBoard& operator=(const ChessBoard& other) {
        if (this != &other) {
            _turn = other._turn;
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    delete _board[i][j]; // Clean up existing piece
                    if (other._board[i][j] != nullptr) {
                        _board[i][j] = copy_piece(other._board[i][j]);
                    } else {
                        _board[i][j] = nullptr;
                    }
                }
            }
        }
        return *this;
    }

    // Get the current turn
    Color get_turn() const {
        return _turn;
    }


    void print_board() {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (_board[i][j] != nullptr) {
                    std::cout << char(_board[i][j]->get_fen_char()) << " ";
                } else {
                    std::cout << ". ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << "Current turn: " << (_turn == Color::WHITE ? "White" : "Black") << std::endl;
    }

    bool is_in_check(Color color) {
        for (int rank = 0; rank < 8; ++rank) {
            for (int file = 0; file < 8; ++file) {
                Piece* piece = _board[rank][file];
                if (piece == nullptr || piece->get_color() == color) {
                    continue; // Skip empty squares or pieces of the same color
                }

                for (const Coords& direction : piece->get_directions()) {
                    int x = rank + direction.x;
                    int y = file + direction.y;

                    if (!are_coords_valid(Coords(x, y))) {
                        continue; // Skip out-of-bounds coordinates
                    }

                    if (dynamic_cast<const Pawn*>(piece) || dynamic_cast<const Knight*>(piece) || dynamic_cast<const King*>(piece)) {
                        if (dynamic_cast<const Pawn*>(piece) && direction.y == 0) {
                            continue; // Pawns cannot attack straight ahead
                        }
                        const Piece* target_piece = _board[x][y];
                        if (target_piece != nullptr && dynamic_cast<const King*>(target_piece) && target_piece->get_color() == color) {
                            return true; // Our King is in check
                        }
                    }
                    else {

                        // For sliding pieces (Rook, Bishop, Queen)
                        while (are_coords_valid(Coords(x, y))) {
                            const Piece* target_piece = _board[x][y];
                            if (target_piece != nullptr) {
                                if (dynamic_cast<const King*>(target_piece) && target_piece->get_color() == color) {
                                    return true; // Our King is in check
                                }
                                break; // Stop sliding in this direction (we hit a piece)
                            }
                            x += direction.x;
                            y += direction.y;
                        }
                    }
                }
            }
        }
        return false; // Position is not in check
    }

    bool position_safe_after_move(Piece* piece, const Coords& from, const Coords& to) {

        Piece* new_piece = _board[to.x][to.y];
        if (new_piece != nullptr && new_piece->get_color() == piece->get_color()) {
            return false; // Cannot capture own piece
        }

        // Temporarily move the piece
        this->_board[from.x][from.y] = nullptr;
        this->_board[to.x][to.y] = piece;

        // Check if position is safe
        bool safe = !is_in_check(piece->get_color());

        // Restore the original position
        this->_board[from.x][from.y] = piece;
        this->_board[to.x][to.y] = new_piece;
        
        return safe;
    }

    std::vector<Move> get_valid_moves() {
        std::vector<Move> valid_moves;

        for (int rank = 0; rank < 8; ++rank) {
            for (int file = 0; file < 8; ++file) {
                Piece* piece = _board[rank][file];
                if (piece == nullptr || piece->get_color() != _turn) {
                    continue; // Skip empty squares or pieces of the opposite color
                }

                for (const Coords& direction : piece->get_directions()) {
                    int x = rank + direction.x;
                    int y = file + direction.y;

                    if (!are_coords_valid(Coords(x, y))) {
                        continue; // Skip out-of-bounds coordinates
                    }

                    Piece* target_piece = _board[x][y];
                    if (target_piece != nullptr && target_piece->get_color() == piece->get_color()) {
                        continue; // Cannot capture own piece
                    }

                    // Need to restrict pawn moves in certain directions
                    if (dynamic_cast<const Pawn*>(piece)) {

                        // Cannot move two pieces forward if there's a piece in front or if the target square is occupied
                        if (direction.x == 2 || direction.x == -2) {
                            if (target_piece != nullptr) { continue; }
                            if (_board[rank + (direction.x == 2 ? 1 : -1)][file] != nullptr) { continue; }
                        }

                        // Cannot move pawn forward if there's a piece in front
                        else if ((direction.x == 1 || direction.x == -1) && direction.y == 0) {
                            if (target_piece != nullptr) { continue; }
                        }

                        // Cannot capture diagonally if there's no piece in that direction or friendly
                        else if (direction.y != 0 && (target_piece == nullptr || target_piece->get_color() == piece->get_color())) { continue; }
                    }

                    // For non-sliding pieces (Pawn, Knight, King)
                    if (dynamic_cast<const Pawn*>(piece) || dynamic_cast<const Knight*>(piece) || dynamic_cast<const King*>(piece)) {
                        Move move(Coords(rank, file), Coords(x, y));
                        if (position_safe_after_move(piece, Coords(rank, file), Coords(x, y))) {
                            valid_moves.push_back(move);
                        }
                        continue; // Skip to the next direction
                    }

                    // For sliding pieces (Rook, Bishop, Queen)
                    else {
                        while (are_coords_valid(Coords(x, y))) {
                            Piece* target_piece = _board[x][y];
    
                            if (target_piece != nullptr) {
                                if (target_piece->get_color() != piece->get_color()) {
                                    // Capture move
                                    Move move(Coords(rank, file), Coords(x, y));
                                    if (position_safe_after_move(piece, Coords(rank, file), Coords(x, y))) {
                                        valid_moves.push_back(move);
                                    }
                                }
                                break; // Stop sliding in this direction (we hit a piece)
                            } else {
                                // Regular move
                                Move move(Coords(rank, file), Coords(x, y));
                                if (position_safe_after_move(piece, Coords(rank, file), Coords(x, y))) {
                                    valid_moves.push_back(move);
                                }
                            }
                            x += direction.x;
                            y += direction.y;
                        }
                    }
                }
            }
        }

        return valid_moves;
    }

    std::map<std::string, std::vector<Coords>> get_safe_squares() {
        std::map<std::string, std::vector<Coords>> safe_squares;

        for (int rank = 0; rank < 8; ++rank) {
            for (int file = 0; file < 8; ++file) {
                Piece* piece = _board[rank][file];
                if (piece == nullptr || piece->get_color() != _turn) {
                    continue; // Skip empty squares or pieces of the opposite color
                }
                
                for (const Coords& direction : piece->get_directions()) {
                    int x = rank + direction.x;
                    int y = file + direction.y;

                    if (!are_coords_valid(Coords(x, y))) {
                        continue; // Skip out-of-bounds coordinates
                    }

                    Piece* target_piece = _board[x][y];
                    if (target_piece != nullptr && target_piece->get_color() == piece->get_color()) {
                        continue; // Cannot capture own piece
                    }

                    // Need to restrict pawn moves in certain directions
                    if (dynamic_cast<const Pawn*>(piece)) {

                        // Cannot move two pieces forward if there's a piece in front or if the target square is occupied
                        if (direction.x == 2 || direction.x == -2) {
                            if (target_piece != nullptr) { continue; }
                            if (_board[rank + (direction.x == 2 ? 1 : -1)][file] != nullptr) { continue; }
                        }

                        // Cannot move pawn forward if there's a piece in front
                        else if ((direction.x == 1 || direction.x == -1) && direction.y == 0) {
                            if (target_piece != nullptr) { continue; }
                        }

                        // Cannot capture diagonally if there's no piece in that direction or friendly
                        else if (direction.y != 0 && (target_piece == nullptr || target_piece->get_color() == piece->get_color())) { continue; }
                    }

                    // For non-sliding pieces (Pawn, Knight, King)
                    if (dynamic_cast<const Pawn*>(piece) || dynamic_cast<const Knight*>(piece) || dynamic_cast<const King*>(piece)) {
                        Move move(Coords(rank, file), Coords(x, y));
                        if (position_safe_after_move(piece, Coords(rank, file), Coords(x, y))) {
                            std::string key = std::to_string(rank) + ',' + std::to_string(file);
                            safe_squares[key].push_back(Coords(x, y));
                        }
                        continue; // Skip to the next direction
                    }

                    // For sliding pieces (Rook, Bishop, Queen)
                    else {
                        while (are_coords_valid(Coords(x, y))) {
                            Piece* target_piece = _board[x][y];
    
                            if (target_piece != nullptr) {
                                if (target_piece->get_color() != piece->get_color()) {
                                    // Capture move
                                    Move move(Coords(rank, file), Coords(x, y));
                                    if (position_safe_after_move(piece, Coords(rank, file), Coords(x, y))) {
                                        std::string key = std::to_string(rank) + ',' + std::to_string(file);
                                        safe_squares[key].push_back(Coords(x, y));
                                    }
                                }
                                break; // Stop sliding in this direction (we hit a piece)
                            } else {
                                // Regular move
                                Move move(Coords(rank, file), Coords(x, y));
                                if (position_safe_after_move(piece, Coords(rank, file), Coords(x, y))) {
                                    std::string key = std::to_string(rank) + ',' + std::to_string(file);
                                    safe_squares[key].push_back(Coords(x, y));
                                }
                            }
                            x += direction.x;
                            y += direction.y;
                        }
                    }
                }

                if (dynamic_cast<const King*>(piece)) {
                    if (can_castle(dynamic_cast<King*>(piece), true)) {
                        std::string key = std::to_string(rank) + ',' + std::to_string(file);
                        safe_squares[key].push_back(Coords(rank, file + 2)); // King-side castling
                    }
                    if (can_castle(dynamic_cast<King*>(piece), false)) {
                        std::string key = std::to_string(rank) + ',' + std::to_string(file);
                        safe_squares[key].push_back(Coords(rank, file - 2)); // Queen-side castling
                    }
                }
            }
        }

        return safe_squares;
    }

    ChessBoard* clone() const {
        return new ChessBoard(*this); // Use copy constructor to create a deep copy
    }

    // Function to make a move on the board
    bool make_move(const Move& move) {
       if (!are_coords_valid(move.from) || !are_coords_valid(move.to)) return false;

        Piece* piece = _board[move.from.x][move.from.y];
        if (piece == nullptr || piece->get_color() != _turn) {
            return false; // No piece at the from position or not the player's turn
        }

        // Check if the move is valid
        std::string key = std::to_string(move.from.x) + ',' + std::to_string(move.from.y);
        if (safe_squares.find(key) == safe_squares.end() || 
            std::find(safe_squares[key].begin(), safe_squares[key].end(), move.to) == safe_squares[key].end()) {
            return false; // Move is not valid
        }


        // Handle special moves
        handle_special_move(piece, move.from, move.to);

        // Move the piece
        delete _board[move.to.x][move.to.y]; // Capture any piece at the target square
        _board[move.to.x][move.to.y] = piece;
        _board[move.from.x][move.from.y] = nullptr; // Clear the from square
        if (dynamic_cast<King*>(piece)) {
            dynamic_cast<King*>(piece)->set_has_moved(); // Mark the King as having moved
        } else if (dynamic_cast<Rook*>(piece)) {
            dynamic_cast<Rook*>(piece)->set_has_moved(); // Mark the Rook as having moved
        } else if (dynamic_cast<Pawn*>(piece)) {
            dynamic_cast<Pawn*>(piece)->set_has_moved(); // Mark the Pawn as having moved
        }

        // Switch turn
        _turn = (_turn == Color::WHITE) ? Color::BLACK : Color::WHITE;
        safe_squares = get_safe_squares(); // Update safe squares after the move

        return true; // Move was successful
    }


    bool can_castle(King* k, bool king_side_castle) {
        if (k->get_has_moved()) {
            return false; // King has already moved, cannot castle
        }

        const Coords& king_pos = (k->get_color() == Color::WHITE) ? Coords(0, 4) : Coords(7, 4);
        const Coords& rook_pos = (king_side_castle) ? Coords(0, 7) : Coords(0, 0);
        if (_board[rook_pos.x][rook_pos.y] == nullptr || 
            !dynamic_cast<Rook*>(_board[rook_pos.x][rook_pos.y]) || 
            dynamic_cast<Rook*>(_board[rook_pos.x][rook_pos.y])->get_has_moved()) {
            return false; // Rook is not present or has already moved
        }

        const Coords& first_square = (king_side_castle) ? king_pos + Coords(0, 1) : king_pos + Coords(0, -1);
        const Coords& second_square = (king_side_castle) ? king_pos + Coords(0, 2) : king_pos + Coords(0, -2);
        if (_board[first_square.x][first_square.y] != nullptr || 
            _board[second_square.x][second_square.y] != nullptr) {
            return false; // Squares between King and Rook are not empty
        }

        // Check if the squares are safe
        if (!position_safe_after_move(k, king_pos, first_square) || 
            !position_safe_after_move(k, king_pos, second_square)) {
            return false; // Squares are not safe for castling
        }

        if (!king_side_castle) {
            const Coords& third_square = king_pos + Coords(0, -3);
            if (_board[third_square.x][third_square.y] != nullptr) {
                return false; // The square next to the Rook is not empty
            }
            if (!position_safe_after_move(k, king_pos, third_square)) {
                return false; // The square next to the Rook is not safe
            }
        }

       return is_in_check(k->get_color()) == false; // Ensure the King is not in check after castling
    }

    void handle_special_move(Piece* piece, const Coords& from, const Coords& to) {
        
        // Castling move
        if (dynamic_cast<King*>(piece) && abs(from.y - to.y) == 2) {
            if (to.y > from.y) { // King-side castling
                _board[to.x][to.y - 1] = _board[from.x][7]; // Move the Rook
                _board[from.x][7] = nullptr; // Clear the Rook's original square
            } else { // Queen-side castling
                _board[to.x][to.y + 1] = _board[from.x][0]; // Move the Rook
                _board[from.x][0] = nullptr; // Clear the Rook's original square
            }
        }

    }




    private:
    // Helper function to check if a piece can move to a specific square
    bool are_coords_valid(const Coords& coords) const {
        return coords.x >= 0 && coords.x < 8 && coords.y >= 0 && coords.y < 8;
    }
    std::map<std::string, std::vector<Coords>> safe_squares;
};

#endif // CHESS_BOARD_H