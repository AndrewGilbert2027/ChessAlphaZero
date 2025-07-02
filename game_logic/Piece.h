#ifndef PIECE_H
#define PIECE_H
#include "types.h"
#include <vector>

class Piece {
public:
    // Constructors
    Piece(Color color) : _color(color), _fen_char(EMPTY_SQUARE) {}
    Piece(Color color, FENChar fen_char) : _color(color), _fen_char(fen_char) {}

    // Copy constructor
    Piece(const Piece& other) : _color(other._color), _fen_char(other._fen_char), _directions(other._directions) {}

    // Assignment operator
    Piece& operator=(const Piece& other) {
        if (this != &other) {
            _color = other._color;
            _fen_char = other._fen_char;
            _directions = other._directions;
        }
        return *this;
    }

    virtual ~Piece() = default;

    // Getters
    Color get_color() const {
        return _color;
    }

    FENChar get_fen_char() const {
        return _fen_char;
    }

    const std::vector<Coords>& get_directions() const {
        return _directions;
    }

protected:
    Color _color; // Color of the piece (WHITE or BLACK)
    FENChar _fen_char; // FEN character representing the piece type
    std::vector<Coords> _directions; // Directions the piece can move in
};
#endif // PIECE_H