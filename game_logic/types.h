#ifndef TYPES_H
#define TYPES_H

#include <ostream>

enum class Color {
    WHITE = 1,
    BLACK = -1
};

struct Coords {
    int x; // Row index (0-7)
    int y; // Column index (0-7)
    Coords(int x = 0, int y = 0) : x(x), y(y) {}
    Coords(const Coords& other) : x(other.x), y(other.y) {}
    Coords& operator=(const Coords& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }
    bool operator==(const Coords& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Coords& other) const {
        return !(*this == other);
    }
    Coords operator+(const Coords& other) const {
        return Coords(x + other.x, y + other.y);
    }
    Coords operator-(const Coords& other) const {
        return Coords(x - other.x, y - other.y);
    }
    Coords operator*(int scalar) const {
        return Coords(x * scalar, y * scalar);
    }
    Coords operator/(int scalar) const {
        return Coords(x / scalar, y / scalar);
    }
    Coords& operator+=(const Coords& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Coords& operator-=(const Coords& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Coords& operator*=(int scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Coords& coords) {
    os << "(" << coords.x << "," << coords.y << ")";
    return os;
}

struct Move {
    Coords from; // Starting position of the piece
    Coords to;   // Target position of the piece
    Move(const Coords& from = Coords(), const Coords& to = Coords()) : from(from), to(to) {}
    Move(int from_x, int from_y, int to_x, int to_y) 
        : from(Coords(from_x, from_y)), to(Coords(to_x, to_y)) {}
    
    bool operator==(const Move& other) const {
        return from == other.from && to == other.to;
    }
    friend std::ostream& operator<<(std::ostream& os, const Move& move);

    operator std::tuple<int, int, int, int>() const {
        return std::make_tuple(from.x, from.y, to.x, to.y);
    }

    std::tuple<int, int, int, int> unpack() const {
        return std::make_tuple(from.x, from.y, to.x, to.y);
    }
};

inline std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << "Move from (" << move.from.x << "," << move.from.y << ") to (" 
       << move.to.x << "," << move.to.y << ")";
    return os;
}

enum FENChar {
    WhitePAWN = 'P',
    BlackPAWN = 'p',
    WhiteROOK = 'R',
    BlackROOK = 'r',
    WhiteKNIGHT = 'N',
    BlackKNIGHT = 'n',
    WhiteBISHOP = 'B',
    BlackBISHOP = 'b',
    WhiteQUEEN = 'Q',
    BlackQUEEN = 'q',
    WhiteKING = 'K',
    BlackKING = 'k',
    EMPTY_SQUARE = '.',
    NULL_SQUARE = '\0'
};

#endif // TYPES_H