#ifndef BISHOP_H
#define BISHOP_H
#include "Piece.h"

class Bishop : public Piece {
public:
    Bishop(Color color) : Piece(color, (color == Color::WHITE ? WhiteBISHOP : BlackBISHOP)) {
        _directions = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    }

    // Copy constructor
    Bishop(const Bishop& other) : Piece(other) {}

    // Assignment operator
    Bishop& operator=(const Bishop& other) {
        if (this != &other) {
            Piece::operator=(other);
        }
        return *this;
    }
};

#endif // BISHOP_H
