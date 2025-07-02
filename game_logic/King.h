#ifndef KING_H
#define KING_H
#include "Piece.h"

class King : public Piece {
public:
    King(Color color) : Piece(color, (color == Color::WHITE ? WhiteKING : BlackKING)) {
        _directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0},
                       {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    }

    // Copy constructor
    King(const King& other) : Piece(other), has_moved(other.has_moved) {}

    // Assignment operator
    King& operator=(const King& other) {
        if (this != &other) {
            Piece::operator=(other);
            has_moved = other.has_moved;
        }
        return *this;
    }

    bool get_has_moved() const { return has_moved; }
    void set_has_moved() { has_moved = true; }

private:
    bool has_moved = false;
};

#endif // KING_H
