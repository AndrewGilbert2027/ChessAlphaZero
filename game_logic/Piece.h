#ifndef PIECE_H
#define PIECE_H
#include "types.h"
#include <vector>

class Piece {
public:
    // Constructors
    Piece(Color color, char type) : _color(color), _fen_char(EMPTY_SQUARE), _type(type) {}
    Piece(Color color, FENChar fen_char, char type) : _color(color), _fen_char(fen_char), _type(type) {}

    // Copy constructor
    Piece(const Piece& other) : _color(other._color), _fen_char(other._fen_char), _type(other._type), _directions(other._directions) {}

    // Assignment operator
    Piece& operator=(const Piece& other) {
        if (this != &other) {
            _color = other._color;
            _fen_char = other._fen_char;
            _directions = other._directions;
            _type = other._type;
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

    char get_type() const {
        return _type;
    }

    const std::vector<Coords>& get_directions() const {
        return _directions;
    }

    virtual bool get_has_moved() const { return false; }
    virtual void set_has_moved() {}

protected:
    Color _color; // Color of the piece (WHITE or BLACK)
    FENChar _fen_char; // FEN character representing the piece type
    char _type; // a character representing the piece type, e.g. 'p' for pawn
    std::vector<Coords> _directions; // Directions the piece can move in
};
#endif // PIECE_H