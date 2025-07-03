#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include "Piece.h"
#include "types.h"
#include <vector>
#include <map>
#include <string>

class Pawn;
class Rook;
class King;

class ChessBoard {
public:
    /**
     * @brief Default constructor. Initializes the board to the standard starting chess position.
     */
    ChessBoard();

    /**
     * @brief Destructor. Cleans up all dynamically allocated Piece objects on the board.
     */
    ~ChessBoard();

    /**
     * @brief Copy constructor. Creates a deep copy of another ChessBoard object.
     * @param other The ChessBoard object to copy.
     */
    ChessBoard(const ChessBoard& other);

    /**
     * @brief Assignment operator. Replaces the current board state with a deep copy of another board.
     * @param other The ChessBoard object to assign from.
     * @return A reference to this ChessBoard instance.
     */
    ChessBoard& operator=(const ChessBoard& other);

    /**
     * @brief Gets the color of the player whose turn it is.
     * @return The current turn's color (Color::WHITE or Color::BLACK).
     */
    Color get_turn() const;

    /**
     * @brief Creates a new ChessBoard state by applying a move.
     * This is useful for simulations or AI algorithms that need to explore future states.
     * @param move The move to apply.
     * @return A pointer to a new ChessBoard object representing the state after the move. Returns nullptr if the move is invalid. The caller is responsible for deleting the returned object.
     */
    ChessBoard* step(const Move& move);

    /**
     * @brief Resets the board to the standard initial chess setup.
     */
    void reset();

    /**
     * @brief Prints a simple character-based representation of the board to the console.
     */
    void print_board();

    /**
     * @brief Checks if a king of a given color is currently in check.
     * @param color The color of the king to check.
     * @return True if the king is in check, false otherwise.
     */
    bool is_in_check(Color color);

    /**
     * @brief Simulates a move to see if it would leave the king in a safe position.
     * @param piece The piece to move.
     * @param from The starting coordinates of the move.
     * @param to The ending coordinates of the move.
     * @return True if the king of the piece's color is not in check after the move, false otherwise.
     */
    bool position_safe_after_move(Piece* piece, const Coords& from, const Coords& to);

    /**
     * @brief Generates a list of all legal moves for the current player.
     * @return A std::vector of Move objects representing all valid moves.
     */
    std::vector<Move> get_valid_moves();

    /**
     * @brief Checks if the game has ended (checkmate, stalemate, etc.).
     * @return True if the game is over, false otherwise.
     */
    bool is_game_over() const;

    /**
     * @brief Updates the game over status by checking for checkmate, stalemate, and other draw conditions.
     */
    void check_game_over();

    /**
     * @brief Gets the outcome of the game if it has ended.
     * @return 1 for White win, -1 for Black win, 0 for draw. Value is undefined if game is not over.
     */
    int get_outcome() const;

    /**
     * @brief Checks for insufficient material to continue the game (e.g., King vs. King).
     * @return True if material is insufficient for a checkmate, false otherwise.
     */
    bool insufficient_material();

    /**
     * @brief Gets a map of all legal moves for the current player, organized by piece.
     * The key is a string representation of the piece's coordinates.
     * @return A map where keys are "rank,file" and values are vectors of valid destination coordinates.
     */
    std::map<std::string, std::vector<Coords>> get_safe_squares();

    /**
     * @brief Creates a deep copy of the current ChessBoard object.
     * @return A pointer to the newly created ChessBoard. The caller is responsible for deleting this object.
     */
    ChessBoard* clone() const;

    /**
     * @brief Applies a move to the board, updating piece positions and game state.
     * @param move The move to be made.
     * @return True if the move was legal and successfully made, false otherwise.
     */
    bool make_move(const Move& move);

    /**
     * @brief Returns the board state as a 2D vector of characters (FEN notation).
     * @return An 8x8 vector of chars representing the pieces on the board.
     */
    std::vector<std::vector<char>> get_board_state_chars() const;

    Move random_move() {
        std::vector<Move> moves = get_valid_moves();
        if (moves.empty()) {
            return Move(); // Return an empty move if no valid moves are available
        }
        return moves[rand() % moves.size()]; // Randomly select a move from the valid moves
    }

private:
    Piece* _board[8][8]; // 8x8 board of pieces
    Color _turn;         // Current turn (WHITE or BLACK)
    std::map<std::string, std::vector<Coords>> safe_squares; // Map of safe squares for the current turn
    Piece* last_piece;
    Move last_move;
    bool _game_over;
    int fifty_move_rule_counter = 0;
    int outcome;

    /**
     * @brief Checks if a given set of coordinates is within the bounds of the 8x8 board.
     * @param coords The coordinates to check.
     * @return True if the coordinates are valid, false otherwise.
     */
    bool are_coords_valid(const Coords& coords) const;

    /**
     * @brief Checks if an en passant capture is possible for a given pawn.
     * @param p The pawn to check.
     * @param pCoords The coordinates of the pawn.
     * @return True if en passant is a legal move, false otherwise.
     */
    bool can_en_passant(Pawn* p, Coords pCoords);

    /**
     * @brief Checks if castling is a legal move for a given king.
     * @param k The king to check.
     * @param king_side_castle True to check for king-side castling, false for queen-side.
     * @return True if the specified castling move is legal, false otherwise.
     */
    bool can_castle(King* k, bool king_side_castle);

    /**
     * @brief Handles the execution of special moves like castling and en passant.
     * @param piece The piece being moved.
     * @param from The starting coordinates of the move.
     * @param to The ending coordinates of the move.
     */
    void handle_special_move(Piece* piece, const Coords& from, const Coords& to);

    /**
     * @brief Helper function to check if a list of pieces contains a bishop or a knight.
     * @param pieces The list of pieces to check.
     * @return True if a minor piece is found, false otherwise.
     */
    bool containsMinorPiece(const std::vector<std::pair<Piece*, Coords>>& pieces);
};

#endif // CHESS_BOARD_H