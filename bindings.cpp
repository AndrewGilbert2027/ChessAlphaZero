// bindings.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // For automatic STL conversions
#include "game_logic/ChessBoard.h" 
#include "game_logic/Piece.h"
#include "game_logic/types.h"

namespace py = pybind11;

PYBIND11_MODULE(chessengine, m) {
    m.doc() = "Chess Engine Module";

    // Bind Coords class
    py::class_<Coords>(m, "Coords")
        .def(py::init<int, int>())
        .def_readwrite("x", &Coords::x)
        .def_readwrite("y", &Coords::y);

    // Bind Move class
    py::class_<Move>(m, "Move")
        .def(py::init<Coords, Coords>())
        .def_readwrite("start", &Move::from)
        .def_readwrite("to", &Move::to);

    // Bind Color enum
    py::enum_<Color>(m, "Color")
        .value("WHITE", Color::WHITE)
        .value("BLACK", Color::BLACK)
        .export_values();

    // Bind ChessBoard class
    py::class_<ChessBoard>(m, "ChessBoard")
        .def(py::init<>())
        .def("make_move", &ChessBoard::make_move, "Make a move on the chessboard")
        .def("get_turn", &ChessBoard::get_turn, "Get the current turn")
        .def("is_in_check", &ChessBoard::is_in_check, "Check if a color is in check")
        .def("print_board", &ChessBoard::print_board, "Print the current state of the chessboard")
        .def("clone", &ChessBoard::clone, "Create a deep copy of the chessboard")
        .def("get_board_state_chars", &ChessBoard::get_board_state_chars, 
             "Get the current state of the chessboard as a 2D array of characters")
        .def("get_valid_moves", &ChessBoard::get_valid_moves, 
             "Get all valid moves for the current turn")
        .def("get_safe_squares", &ChessBoard::get_safe_squares, 
             "Get all safe squares for the current turn")
        .def("is_game_over", &ChessBoard::is_game_over, 
             "Check if the game is over")
        .def("get_outcome", &ChessBoard::get_outcome, 
             "Get the outcome of the game (checkmate, stalemate, etc.)");
}