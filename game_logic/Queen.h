#ifndef QUEEN_H
#define QUEEN_H
#include "Piece.h"

class Queen : public Piece {
public:
    Queen(Color color) : Piece(color, (color == Color::WHITE ? WhiteQUEEN : BlackQUEEN), 'q') {
        _directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0},
                       {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    }

    // Copy constructor
    Queen(const Queen& other) : Piece(other) {}

    // Assignment operator
    Queen& operator=(const Queen& other) {
        if (this != &other) {
            Piece::operator=(other);
        }
        return *this;
    }
};

#endif // QUEEN_H
