# ChessBot Project

## Description

ChessBot is a sophisticated chess application featuring a powerful C++ chess engine, a user-friendly GUI built with Pygame, and an intelligent AI opponent powered by a Monte Carlo Tree Search (MCTS) algorithm and a PyTorch-based neural network. The C++ core is exposed to Python using `pybind11`, allowing for high performance and easy integration with Python libraries. 

This project was intended as a proof of concept and also to further gain familiarity with exposing c++ code with bindings (something that is very popular and helpful). Future projects could use the stockfish optimizations to further increase performance and speed. 

## Project Structure

The project is organized as follows:
- **README.md**: This file.
- **game_logic/**: Contains the C++ source code for the core chess engine (`ChessBoard.h`, `ChessBoard.cpp`, etc.).
- **bindings.cpp**: Contains the `pybind11` bindings to expose the C++ chess engine to Python.
- **setup.py**: The build script used to compile the Python bindings.
- **Makefile**: Simplifies the compilation process.
- **gui.py**: A Pygame-based graphical user interface to play the game.
- **Game.py**: A Python wrapper class for the C++ `ChessBoard` object, providing an interface compatible with the MCTS logic.
- **MCTS.py**: The implementation of the Monte Carlo Tree Search algorithm.
- **Model.py**: The `ChessCNN` neural network model implemented in PyTorch.
- **images/**: Contains the PNG images for the chess pieces.

## Requirements

To run this project, you need the following installed:
- A C++17 compatible compiler (e.g., g++)
- Python 3.8+
- Pybind11
- Pygame
- PyTorch
- NumPy

You can install the required Python packages using pip:
```bash
pip install pybind11 pygame torch numpy
```

## Setup and Usage

1.  **Compile the C++ Bindings:**
    Run the following command in the project's root directory. This will compile the C++ source code into a Python module.
    ```bash
    make compile-python-bindings
    ```

2. **Test c++ compile and bindings:**
    Go to the game_logic directory and run the following command to see performance of C++ chess engine. 
    ```bash
    make time_test
    ./time_test
    ```
    Afterwards, redirect to main directory and run the following command to see if bindings work.
    ```bash
    python test.py
    ```

2.  **Run the GUI:**
    After compiling the bindings, you can start the game by running the GUI script.
    ```bash
    python gui.py
    ```

## Future Expansion

Possible avenues for expanding the ChessBot project include:
- **Full C++ integration**: To even further speed up training, implement the monte carlo tree search algorithm in c++. 
- **More game logic**: Add code to handle promotions with the policy head and also logic that ends a game if there is a loop. 
- **Training Pipeline**: Implement a full AlphaZero-style training loop to improve the neural network model through self-play.
- **Advanced GUI Features**: Add features like game analysis, move suggestions, and the ability to save/load games.
- **Multiplayer Support**: Add network capabilities for online multiplayer chess games.
- **Performance Optimization**: Further optimize the C++ engine and MCTS implementation.
