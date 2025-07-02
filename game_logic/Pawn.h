#ifndef PAWN_H
#define PAWN_H
#include "Piece.h"

class Pawn : public Piece {
public:
    Pawn(Color color) : Piece(color, (color == Color::WHITE ? WhitePAWN : BlackPAWN)) {
        _directions = (color == Color::WHITE) ? std::vector<Coords>{{1, 0}, {2, 0}, {1, 1}, {1, -1}}
                                       : std::vector<Coords>{{-1, 0}, {-2, 0}, {-1, 1}, {-1, -1}};
    }

    // Copy constructor
    Pawn(const Pawn& other) : Piece(other), has_moved(other.has_moved) {}

    // Assignment operator
    Pawn& operator=(const Pawn& other) {
        if (this != &other) {
            Piece::operator=(other);
            has_moved = other.has_moved;
            if (has_moved == true) {
                this->set_has_moved();
            }
        }
        return *this;
    }

    bool get_has_moved() const { return has_moved; }
    void set_has_moved() {
        has_moved = true;
        _directions = (get_color() == Color::WHITE) ? std::vector<Coords>{{1, 0}, {1, 1}, {1, -1}}
                                             : std::vector<Coords>{{-1, 0}, {-1, 1}, {-1, -1}};
    }

private:
    bool has_moved = false;
};

#endif // PAWN_H
