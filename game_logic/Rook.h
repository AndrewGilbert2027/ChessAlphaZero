#ifndef ROOK_H
#define ROOK_H
#include "Piece.h"

class Rook : public Piece {
public:
    Rook(Color color) : Piece(color, (color == Color::WHITE ? WhiteROOK : BlackROOK), 'r') {
        _directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    }

    // Copy constructor
    Rook(const Rook& other) : Piece(other), has_moved(other.has_moved) {}

    // Assignment operator
    Rook& operator=(const Rook& other) {
        if (this != &other) {
            Piece::operator=(other);
            has_moved = other.has_moved;
        }
        return *this;
    }

    bool get_has_moved() const override { return has_moved; }
    void set_has_moved() override { has_moved = true; }

private:
    bool has_moved = false;
};

#endif // ROOK_H
