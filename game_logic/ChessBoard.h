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

        last_piece = nullptr; // Initialize last piece to nullptr
        last_move = Move(Coords(-1, -1), Coords(-1, -1)); // Initialize last move to invalid coordinates
        _game_over = false;
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

    ChessBoard* step(const Move& move) {
        ChessBoard* new_board = this->clone(); // Create a deep copy of the current board
        new_board->make_move(move); // Apply the move to the new board
        return new_board; // Return the new board
    }

    void reset() {
        // Reset the board to the initial state
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                delete _board[i][j]; // Clean up existing pieces
                _board[i][j] = nullptr; // Set to nullptr
            }
        }
        // Reinitialize the board
        *this = ChessBoard();
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
                if (dynamic_cast<const King*>(piece)) {
                    if (can_castle(dynamic_cast<King*>(piece), true)) {
                        valid_moves.push_back(Move(Coords(rank, file), Coords(rank, file + 2))); // King-side castling
                    }
                    if (can_castle(dynamic_cast<King*>(piece), false)) {
                        valid_moves.push_back(Move(Coords(rank, file), Coords(rank, file -2)));  // Queen-side castling
                    }
                } else if (dynamic_cast<Pawn*>(piece)) {
                    Pawn* pawn = dynamic_cast<Pawn*>(piece);
                    Coords pawn_coords(rank, file);
                    if (can_en_passant(pawn, pawn_coords)) {
                        // En passant move
                        int en_passant_x = (pawn->get_color() == Color::WHITE) ? rank + 1 : rank - 1;
                        int en_passant_y = last_move.to.y;
                        valid_moves.push_back(Move(Coords(rank, file), Coords(en_passant_x, en_passant_y)));
                    }
                }
            }
        }

        return valid_moves;
    }

    bool is_game_over() const {
        return _game_over;
    }

    void check_game_over() {
        // Check if the current player has any valid moves
        if (this->safe_squares.size() == 0) {
            // If the current player has no valid moves, check if they are in check
            if (is_in_check(_turn)) {
                this->_game_over = true; // Checkmate
                this->outcome = (_turn == Color::WHITE) ? -1 : 1; // Set outcome based on the current turn (black wins (outcome -1))
                std::cout << "Checkmate! " << (_turn == Color::WHITE ? "Black" : "White") << " wins!" << std::endl;
            } else {
                _game_over = true; // Stalemate
                this->outcome = 0; // Set outcome to draw
                std::cout << "Stalemate! The game is a draw." << std::endl;
            }
        }
        if (fifty_move_rule_counter >= 100) {
            _game_over = true; // Fifty-move rule
            this->outcome = 0; // Set outcome to draw
            std::cout << "Fifty-move rule! The game is a draw." << std::endl;
        } else if (insufficient_material()) {
            _game_over = true; // Insufficient material
            this->outcome = 0; // Set outcome to draw
            std::cout << "Insufficient material! The game is a draw." << std::endl;
        }
    }

    int get_outcome() const {
        return this->outcome;
    }

    bool containsMinorPiece(std::vector<std::pair<Piece*, Coords>> pieces) {
        for (auto const& [piece, coord] : pieces) {
            if (dynamic_cast<Bishop*>(piece) || dynamic_cast<Knight*>(piece)) {
                return true; // Found a minor piece
            }
        }
        return false; // No minor pieces found
    }

    bool insufficient_material() {
        std::vector<std::pair<Piece*, Coords>> white_pieces;
        std::vector<std::pair<Piece*, Coords>> black_pieces;

        for (int rank = 0; rank < 8; ++rank) {
            for (int file = 0; file < 8; ++file) {
                Piece* piece = _board[rank][file];
                if (piece != nullptr) {
                    if (piece->get_color() == Color::WHITE) {
                        white_pieces.push_back({piece, Coords(rank, file)});
                    } else {
                        black_pieces.push_back({piece, Coords(rank, file)});
                    }
                }
            }
        }

        // Check for insufficient material conditions
        // King vs King
        if (white_pieces.size() == 1 && black_pieces.size() == 1) {
            return true; // Only kings left
        }

        // King vs King + minor piece (Bishop or Knight)
        if (white_pieces.size() == 2 && black_pieces.size() == 1) {
            if (containsMinorPiece(white_pieces)) {
                return true;
            }
        }

        if (black_pieces.size() == 2 && white_pieces.size() == 1) {
            if (containsMinorPiece(black_pieces)) {
                return true;
            }
        }

        // King and Bishop vs King and Bishop (bishops on same colored squares)
        if (white_pieces.size() == 2 && black_pieces.size() == 2) {
            std::pair<Piece*, Coords> white_non_king = {nullptr, {}};
            for(const auto& p : white_pieces) {
                if (!dynamic_cast<King*>(p.first)) {
                    white_non_king = p;
                    break;
                }
            }

            std::pair<Piece*, Coords> black_non_king = {nullptr, {}};
            for(const auto& p : black_pieces) {
                if (!dynamic_cast<King*>(p.first)) {
                    black_non_king = p;
                    break;
                }
            }

            if (dynamic_cast<Bishop*>(white_non_king.first) && dynamic_cast<Bishop*>(black_non_king.first)) {
                bool white_bishop_is_light = (white_non_king.second.x + white_non_king.second.y) % 2 == 0;
                bool black_bishop_is_light = (black_non_king.second.x + black_non_king.second.y) % 2 == 0;
                if (white_bishop_is_light == black_bishop_is_light) {
                    return true; // Bishops on same color squares
                }
            }
        }

        // Two Knights + King vs King
        if (white_pieces.size() == 3 && black_pieces.size() == 1) {
            int knight_count = 0;
            for (const auto& p : white_pieces) {
                if (dynamic_cast<Knight*>(p.first)) {
                    knight_count++;
                }
            }
            if (knight_count == 2) {
                return true; // Two knights vs king
            }
        }
        if (black_pieces.size() == 3 && white_pieces.size() == 1) {
            int knight_count = 0;
            for (const auto& p : black_pieces) {
                if (dynamic_cast<Knight*>(p.first)) {
                    knight_count++;
                }
            }
            if (knight_count == 2) {
                return true; // Two knights vs king
            }
        }

        return false; // Material is sufficient for a checkmate
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
                } else if (dynamic_cast<Pawn*>(piece)) {
                    Pawn* pawn = dynamic_cast<Pawn*>(piece);
                    Coords pawn_coords(rank, file);
                    if (can_en_passant(pawn, pawn_coords)) {
                        // En passant move
                        int en_passant_x = (pawn->get_color() == Color::WHITE) ? rank + 1 : rank - 1;
                        int en_passant_y = last_move.to.y;
                        std::string key = std::to_string(rank) + ',' + std::to_string(file);
                        safe_squares[key].push_back(Coords(en_passant_x, en_passant_y));
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

        // Update the board
        if (_board[move.to.x][move.to.y] != nullptr || dynamic_cast<Pawn*>(piece)) {
            fifty_move_rule_counter = 0; // Reset the fifty-move rule counter if a piece is captured
        } else {
            fifty_move_rule_counter++; // Increment the fifty-move rule counter if no piece is captured
        }
        delete _board[move.to.x][move.to.y]; // Delete the piece at the target square if it exists
        _board[move.to.x][move.to.y] = piece;

        _board[move.from.x][move.from.y] = nullptr; // Clear the from square
        if (dynamic_cast<King*>(piece)) {
            dynamic_cast<King*>(piece)->set_has_moved(); // Mark the King as having moved
        } else if (dynamic_cast<Rook*>(piece)) {
            dynamic_cast<Rook*>(piece)->set_has_moved(); // Mark the Rook as having moved
        } else if (dynamic_cast<Pawn*>(piece)) {
            dynamic_cast<Pawn*>(piece)->set_has_moved(); // Mark the Pawn as having moved
            if (move.to.x == 7 || move.to.x == 0) {
                // Handle pawn promotion
                delete _board[move.to.x][move.to.y]; // Delete the pawn at the target square
                piece = new Queen(piece->get_color()); // Create a new Queen piece
                _board[move.to.x][move.to.y] = piece; // Place the new Queen at the target square
            }
        }

        // Switch turn
        _turn = (_turn == Color::WHITE) ? Color::BLACK : Color::WHITE;
        last_piece = piece; // Update last piece
        last_move = move; // Update last move
        safe_squares = get_safe_squares(); // Update safe squares after the move
        check_game_over(); // Check if the game is over after the move

        return true; // Move was successful
    }

    bool can_en_passant(Pawn* p, Coords pCoords) {
        if (p == nullptr || p->get_color() != _turn) {
            return false; // Not a pawn or not the player's turn
        }
        if (last_piece == nullptr) {
            return false; // No last piece to check against
        }

        int prevX = last_move.from.x;
        int currX = last_move.to.x;
        int currY = last_move.to.y;
        if (
            dynamic_cast<Pawn*>(last_piece) == nullptr ||
            abs(prevX - currX) != 2 || // Last move must be a two-square pawn advance
            last_piece->get_color() == _turn || // Last piece must be of the opposite color
            currX != pCoords.x || // Last move must be in the same column as the pawn
            abs(currY - pCoords.y) != 1 // Last move must be adjacent to the pawn
        ) return false;

        // Check if the pawn can safely move to the en passant square
        this->_board[currX][currY] = nullptr; 
        int nPawnX = (p->get_color() == Color::WHITE) ? pCoords.x + 1 : pCoords.x - 1;
        int nPawnY = pCoords.y;
        Coords nPawnCoords(nPawnX, nPawnY);
        bool safe = this->position_safe_after_move(p, pCoords, nPawnCoords);
        this->_board[currX][currY] = last_piece; // Restore the last piece
        return safe; // Return whether the pawn can safely move to the en passant square
    }


    bool can_castle(King* k, bool king_side_castle) {
        if (k->get_has_moved()) {
            return false; // King has already moved, cannot castle
        }

        const Coords& king_pos = (k->get_color() == Color::WHITE) ? Coords(0, 4) : Coords(7, 4);

        int rank = (k->get_color() == Color::WHITE) ? 0 : 7;
        const Coords& rook_pos = Coords(rank, king_side_castle==true ? 7 : 0);
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
        } else if (dynamic_cast<Pawn*>(piece) && last_piece != nullptr) {
            if (
                dynamic_cast<Pawn*>(last_piece) &&
                abs(last_move.from.x - last_move.to.x) == 2 && // Last move was a two-square pawn advance
                last_piece->get_color() != piece->get_color() && // Last piece is of the opposite color
                abs(last_move.to.x - to.x) == 1 &&
                from.x == last_move.to.x && // Pawn is on same column as the last move 
                abs(from.y - to.y) == 1 && // Pawn is moving diagonally
                to.y == last_move.to.y // Pawn is moving to the same row as the last move
            )
            {
                // En passant capture
                delete _board[last_move.to.x][last_move.to.y]; // Capture the last moved pawn
                _board[last_move.to.x][last_move.to.y] = nullptr; // Clear the captured pawn's square
            }
        }

    }

    // Add inside the public section of the ChessBoard class (This is used for the GUI implementation)
    std::vector<std::vector<char>> get_board_state_chars() const {
        std::vector<std::vector<char>> board_state(8, std::vector<char>(8, '.'));
        
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (_board[i][j] != nullptr) {
                    board_state[i][j] = static_cast<char>(_board[i][j]->get_fen_char());
                } else {
                    board_state[i][j] = '.';
                }
            }
        }
        
        return board_state;
    }

    Piece* promoted_piece(FENChar type) {
        switch (type) {
            case WhiteROOK: return new Rook(Color::WHITE);
            case BlackROOK: return new Rook(Color::BLACK);
            case WhiteKNIGHT: return new Knight(Color::WHITE);
            case BlackKNIGHT: return new Knight(Color::BLACK);
            case WhiteBISHOP: return new Bishop(Color::WHITE);
            case BlackBISHOP: return new Bishop(Color::BLACK);
            case WhiteQUEEN: return new Queen(Color::WHITE);
            case BlackQUEEN: return new Queen(Color::BLACK);
            default: return nullptr; // Invalid piece type
        }
    }




    private:
    // Helper function to check if a piece can move to a specific square
    bool are_coords_valid(const Coords& coords) const {
        return coords.x >= 0 && coords.x < 8 && coords.y >= 0 && coords.y < 8;
    }
    std::map<std::string, std::vector<Coords>> safe_squares;
    Piece* last_piece;
    Move last_move;
    bool _game_over; 
    int fifty_move_rule_counter = 0; // Counter for the fifty-move rule (100 half-moves)
    int outcome; // Not initialized if ongoing, 1 if white wins, -1 if black wins, 0 if draw
};

#endif // CHESS_BOARD_H