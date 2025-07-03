#ifndef KNIGHT_H
#define KNIGHT_H
#include "Piece.h"

class Knight : public Piece {
public:
    Knight(Color color) : Piece(color, (color == Color::WHITE ? WhiteKNIGHT : BlackKNIGHT), 'n') {
        _directions = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2},
                       {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
    }

    // Copy constructor
    Knight(const Knight& other) : Piece(other) {}

    // Assignment operator
    Knight& operator=(const Knight& other) {
        if (this != &other) {
            Piece::operator=(other);
        }
        return *this;
    }
};

#endif // KNIGHT_H
