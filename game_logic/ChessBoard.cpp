#include "ChessBoard.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <pthread.h>

// Helper function to copy a piece based on its type
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

ChessBoard::ChessBoard() {
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

    last_piece = nullptr;
    last_move = Move(Coords(-1, -1), Coords(-1, -1));
    _game_over = false;
    outcome = 0; // Default to draw, will be updated
    this->safe_squares = get_safe_squares();
}

ChessBoard::~ChessBoard() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            delete _board[i][j];
        }
    }
}

ChessBoard::ChessBoard(const ChessBoard& other) {
    _turn = other._turn;
    last_piece = other.last_piece ? copy_piece(other.last_piece) : nullptr;
    last_move = other.last_move;
    _game_over = other._game_over;
    fifty_move_rule_counter = other.fifty_move_rule_counter;
    outcome = other.outcome;

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            _board[i][j] = copy_piece(other._board[i][j]);
        }
    }
    safe_squares = other.safe_squares;
}

ChessBoard& ChessBoard::operator=(const ChessBoard& other) {
    if (this != &other) {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                delete _board[i][j];
                _board[i][j] = copy_piece(other._board[i][j]);
            }
        }
        _turn = other._turn;
        delete last_piece;
        last_piece = other.last_piece ? copy_piece(other.last_piece) : nullptr;
        last_move = other.last_move;
        _game_over = other._game_over;
        fifty_move_rule_counter = other.fifty_move_rule_counter;
        outcome = other.outcome;
        safe_squares = other.safe_squares;
    }
    return *this;
}

Color ChessBoard::get_turn() const {
    return _turn;
}

ChessBoard* ChessBoard::step(const Move& move) {
    ChessBoard* new_board = this->clone();
    if (new_board == nullptr) {
        return nullptr;
    }
    if (!new_board->make_move(move)) {
        delete new_board;
        return nullptr;
    }
    return new_board;
}

void ChessBoard::reset() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            delete _board[i][j];
            _board[i][j] = nullptr;
        }
    }
    // Reinitialize the board by calling constructor logic
    new (this) ChessBoard();
}

void ChessBoard::print_board() {
    for (int i = 7; i >= 0; --i) {
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

bool ChessBoard::is_in_check(Color color) {
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            Piece* piece = _board[rank][file];
            if (piece == nullptr || piece->get_color() == color) {
                continue;
            }

            for (const Coords& direction : piece->get_directions()) {
                int x = rank + direction.x;
                int y = file + direction.y;

                if (!are_coords_valid(Coords(x, y))) {
                    continue;
                }

                char piece_type = piece->get_type();
                if (piece_type == 'p' || piece_type == 'n' || piece_type == 'k') {
                    if (piece_type == 'p' && direction.y == 0) {
                        continue;
                    }
                    const Piece* target_piece = _board[x][y];
                    if (target_piece != nullptr && target_piece->get_type() == 'k' && target_piece->get_color() == color) {
                        return true;
                    }
                } else {
                    while (are_coords_valid(Coords(x, y))) {
                        const Piece* target_piece = _board[x][y];
                        if (target_piece != nullptr) {
                            if (target_piece->get_type() == 'k' && target_piece->get_color() == color) {
                                return true;
                            }
                            break;
                        }
                        x += direction.x;
                        y += direction.y;
                    }
                }
            }
        }
    }
    return false;
}

bool ChessBoard::position_safe_after_move(Piece* piece, const Coords& from, const Coords& to) {
    Piece* target_piece = _board[to.x][to.y];
    if (target_piece != nullptr && target_piece->get_color() == piece->get_color()) {
        return false;
    }

    _board[to.x][to.y] = piece;
    _board[from.x][from.y] = nullptr;

    bool safe = !is_in_check(piece->get_color());

    _board[from.x][from.y] = piece;
    _board[to.x][to.y] = target_piece;
    
    return safe;
}

std::vector<Move> ChessBoard::get_valid_moves() {
    std::vector<Move> valid_moves;
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            Piece* piece = _board[rank][file];
            if (piece == nullptr || piece->get_color() != _turn) {
                continue;
            }

            char piece_type = piece->get_type();
            for (const Coords& direction : piece->get_directions()) {
                int x = rank + direction.x;
                int y = file + direction.y;

                if (!are_coords_valid(Coords(x, y))) {
                    continue;
                }

                Piece* target_piece = _board[x][y];
                if (target_piece != nullptr && target_piece->get_color() == piece->get_color()) {
                    continue;
                }

                if (piece_type == 'p') {
                    if (direction.x == 2 || direction.x == -2) {
                        if (target_piece != nullptr || _board[rank + (direction.x > 0 ? 1 : -1)][file] != nullptr) continue;
                    } else if (direction.y == 0) { // Forward move
                        if (target_piece != nullptr) continue;
                    } else { // Capture
                        if (target_piece == nullptr) continue;
                    }
                }

                if (piece_type == 'p' || piece_type == 'n' || piece_type == 'k') {
                    if (position_safe_after_move(piece, Coords(rank, file), Coords(x, y))) {
                        valid_moves.push_back(Move(Coords(rank, file), Coords(x, y)));
                    }
                } else { // Sliding pieces
                    int current_x = rank + direction.x;
                    int current_y = file + direction.y;
                    while (are_coords_valid(Coords(current_x, current_y))) {
                        if (_board[current_x][current_y] != nullptr) {
                            if (_board[current_x][current_y]->get_color() != _turn) {
                                if (position_safe_after_move(piece, Coords(rank, file), Coords(current_x, current_y))) {
                                    valid_moves.push_back(Move(Coords(rank, file), Coords(current_x, current_y)));
                                }
                            }
                            break;
                        }
                        if (position_safe_after_move(piece, Coords(rank, file), Coords(current_x, current_y))) {
                            valid_moves.push_back(Move(Coords(rank, file), Coords(current_x, current_y)));
                        }
                        current_x += direction.x;
                        current_y += direction.y;
                    }
                }
            }
            if (piece_type == 'k') {
                if (can_castle(static_cast<King*>(piece), true)) {
                    valid_moves.push_back(Move(Coords(rank, file), Coords(rank, file + 2)));
                }
                if (can_castle(static_cast<King*>(piece), false)) {
                    valid_moves.push_back(Move(Coords(rank, file), Coords(rank, file - 2)));
                }
            } else if (piece_type == 'p') {
                if (can_en_passant(static_cast<Pawn*>(piece), Coords(rank, file))) {
                    int en_passant_x = (_turn == Color::WHITE) ? rank + 1 : rank - 1;
                    valid_moves.push_back(Move(Coords(rank, file), Coords(en_passant_x, last_move.to.y)));
                }
            }
        }
    }
    return valid_moves;
}

bool ChessBoard::is_game_over() const {
    return _game_over;
}

void ChessBoard::check_game_over() {
    if (get_valid_moves().empty()) {
        if (is_in_check(_turn)) {
            _game_over = true;
            outcome = (_turn == Color::WHITE) ? -1 : 1;
            std::cout << "Checkmate! " << (_turn == Color::WHITE ? "Black" : "White") << " wins!" << std::endl;
        } else {
            _game_over = true;
            outcome = 0;
            std::cout << "Stalemate! The game is a draw." << std::endl;
        }
    } else if (fifty_move_rule_counter >= 100) {
        _game_over = true;
        outcome = 0;
        std::cout << "Fifty-move rule! The game is a draw." << std::endl;
    } else if (insufficient_material()) {
        _game_over = true;
        outcome = 0;
        std::cout << "Insufficient material! The game is a draw." << std::endl;
    }
}

int ChessBoard::get_outcome() const {
    return outcome;
}

bool ChessBoard::containsMinorPiece(const std::vector<std::pair<Piece*, Coords>>& pieces) {
    for (const auto& p : pieces) {
        char type = p.first->get_type();
        if (type == 'b' || type == 'n') {
            return true;
        }
    }
    return false;
}

bool ChessBoard::insufficient_material() {
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

    if (white_pieces.size() <= 2 && black_pieces.size() <= 2) {
        if (white_pieces.size() == 1 && black_pieces.size() == 1) return true; // K vs K
        if ((white_pieces.size() == 1 && black_pieces.size() == 2 && containsMinorPiece(black_pieces)) ||
            (white_pieces.size() == 2 && black_pieces.size() == 1 && containsMinorPiece(white_pieces))) return true; // K vs K+N or K vs K+B

        if (white_pieces.size() == 2 && black_pieces.size() == 2) {
            Piece* w_piece = nullptr; Coords w_coords;
            for(const auto& p : white_pieces) if(p.first->get_type() != 'k') { w_piece = p.first; w_coords = p.second; break; }
            Piece* b_piece = nullptr; Coords b_coords;
            for(const auto& p : black_pieces) if(p.first->get_type() != 'k') { b_piece = p.first; b_coords = p.second; break; }

            if (w_piece && b_piece && w_piece->get_type() == 'b' && b_piece->get_type() == 'b') {
                if (((w_coords.x + w_coords.y) % 2) == ((b_coords.x + b_coords.y) % 2)) return true; // Bishops on same color
            }
        }
    }
    
    int white_knight_count = 0;
    for(const auto& p : white_pieces) if(p.first->get_type() == 'n') white_knight_count++;
    if(white_pieces.size() == 3 && white_knight_count == 2 && black_pieces.size() == 1) return true;

    int black_knight_count = 0;
    for(const auto& p : black_pieces) if(p.first->get_type() == 'n') black_knight_count++;
    if(black_pieces.size() == 3 && black_knight_count == 2 && white_pieces.size() == 1) return true;

    return false;
}

std::map<std::string, std::vector<Coords>> ChessBoard::get_safe_squares() {
    std::map<std::string, std::vector<Coords>> safe_squares_map;
    std::vector<Move> moves = get_valid_moves();
    for (const auto& move : moves) {
        std::string key = std::to_string(move.from.x) + "," + std::to_string(move.from.y);
        safe_squares_map[key].push_back(move.to);
    }
    return safe_squares_map;
}

ChessBoard* ChessBoard::clone() const {
    return new ChessBoard(*this);
}

bool ChessBoard::make_move(const Move& move) {
    if (!are_coords_valid(move.from) || !are_coords_valid(move.to)) return false;

    Piece* piece = _board[move.from.x][move.from.y];
    if (piece == nullptr || piece->get_color() != _turn) {
        return false;
    }

    std::string key = std::to_string(move.from.x) + "," + std::to_string(move.from.y);
    if (safe_squares.find(key) == safe_squares.end()) return false;
    const auto& moves_for_piece = safe_squares.at(key);
    if (std::find(moves_for_piece.begin(), moves_for_piece.end(), move.to) == moves_for_piece.end()) {
        return false;
    }

    if (_board[move.to.x][move.to.y] != nullptr || piece->get_type() == 'p') {
        fifty_move_rule_counter = 0;
    } else {
        fifty_move_rule_counter++;
    }

    handle_special_move(piece, move.from, move.to);

    delete _board[move.to.x][move.to.y];
    _board[move.to.x][move.to.y] = piece;
    _board[move.from.x][move.from.y] = nullptr;

    piece->set_has_moved();

    if (piece->get_type() == 'p' && (move.to.x == 0 || move.to.x == 7)) {
        delete _board[move.to.x][move.to.y];
        _board[move.to.x][move.to.y] = new Queen(_turn);
    }

    _turn = (_turn == Color::WHITE) ? Color::BLACK : Color::WHITE;
    last_piece = piece;
    last_move = move;
    safe_squares = get_safe_squares();
    check_game_over();

    return true;
}

std::vector<std::vector<char>> ChessBoard::get_board_state_chars() const {
    std::vector<std::vector<char>> board_state(8, std::vector<char>(8, '.'));
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (_board[i][j] != nullptr) {
                board_state[i][j] = static_cast<char>(_board[i][j]->get_fen_char());
            }
        }
    }
    return board_state;
}

bool ChessBoard::are_coords_valid(const Coords& coords) const {
    return coords.x >= 0 && coords.x < 8 && coords.y >= 0 && coords.y < 8;
}

bool ChessBoard::can_en_passant(Pawn* p, Coords pCoords) {
    if (last_piece == nullptr || last_piece->get_type() != 'p' || last_piece->get_color() == _turn) {
        return false;
    }
    if (abs(last_move.from.x - last_move.to.x) == 2 &&
        pCoords.x == last_move.to.x &&
        abs(pCoords.y - last_move.to.y) == 1) {
        
        int en_passant_x = (_turn == Color::WHITE) ? pCoords.x + 1 : pCoords.x - 1;
        Coords to(en_passant_x, last_move.to.y);
        
        // Temporarily perform the move to check for safety
        Piece* captured_pawn = _board[last_move.to.x][last_move.to.y];
        _board[last_move.to.x][last_move.to.y] = nullptr;
        bool is_safe = position_safe_after_move(p, pCoords, to);
        _board[last_move.to.x][last_move.to.y] = captured_pawn; // Restore board
        return is_safe;
    }
    return false;
}

bool ChessBoard::can_castle(King* k, bool king_side_castle) {
    if (k->get_has_moved() || is_in_check(_turn)) {
        return false;
    }

    int rank = (_turn == Color::WHITE) ? 0 : 7;
    int rook_file = king_side_castle ? 7 : 0;
    Piece* rook = _board[rank][rook_file];

    if (rook == nullptr || rook->get_type() != 'r' || rook->get_has_moved()) {
        return false;
    }

    int step = king_side_castle ? 1 : -1;
    for (int file = 4 + step; file != rook_file; file += step) {
        if (_board[rank][file] != nullptr) {
            return false;
        }
    }

    if (!position_safe_after_move(k, Coords(rank, 4), Coords(rank, 4 + step)) ||
        !position_safe_after_move(k, Coords(rank, 4), Coords(rank, 4 + 2 * step))) {
        return false;
    }

    return true;
}

void ChessBoard::handle_special_move(Piece* piece, const Coords& from, const Coords& to) {
    if (piece->get_type() == 'k' && abs(from.y - to.y) == 2) { // Castling
        int rank = from.x;
        if (to.y > from.y) { // King-side
            Piece* rook = _board[rank][7];
            _board[rank][5] = rook;
            _board[rank][7] = nullptr;
            rook->set_has_moved();
        } else { // Queen-side
            Piece* rook = _board[rank][0];
            _board[rank][3] = rook;
            _board[rank][0] = nullptr;
            rook->set_has_moved();
        }
    } else if (piece->get_type() == 'p' && to.y != from.y && _board[to.x][to.y] == nullptr) { // En passant
        int captured_pawn_x = (_turn == Color::WHITE) ? to.x - 1 : to.x + 1;
        delete _board[captured_pawn_x][to.y];
        _board[captured_pawn_x][to.y] = nullptr;
    }
}
