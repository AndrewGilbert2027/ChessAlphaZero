from chessengine import ChessBoard, Move, Color
import numpy as np

class ChessGame:
    """
    Wrapper class for the ChessBoard C++ class. 
    Provides methods to interact with the chess engine and conforms to the interface
    required by the MCTS module.
    """
    def __init__(self, board=None):
        if board:
            self.board = board
        else:
            self.board = ChessBoard()

    @property
    def player(self):
        """Returns the current player (1 for White, -1 for Black)."""
        return 1 if self.board.get_turn() == Color.WHITE else -1

    def get_feature_plane(self):
        """
        Get the current board state as a tensor for the neural network.
        Returns:
            A 3D numpy array representing the board state.
        """
        return np.array(self.board.get_state_tensor()).reshape(9, 8, 8)
    
    def get_policy_mask(self):
        """
        Get the current policy mask.
        Returns:
            A 4D numpy array representing the policy tensor. 
        """
        return np.array(self.board.get_policy_mask()).reshape(8, 8, 8, 8)
    
    def valid_moves(self):
        """
        Returns a list of valid moves for the current player. 
        Each element is a tuple of (from_row, from_col, to_row, to_col).
        """
        moves = self.board.get_valid_moves()
        return [(move.start.x, move.start.y, move.to.x, move.to.y) for move in moves]
    
    def make_move(self, from_row, from_col, to_row, to_col):
        """
        Make a move on the board. This method modifies the internal board state.
        Args:
            from_row, from_col: The starting position of the piece.
            to_row, to_col: The destination position of the piece.
        Returns:
            True if the move was successful, False otherwise.
        """
        move = Move(from_row, from_col, to_row, to_col)
        return self.board.make_move(move)

    def step(self, action):
        """
        Applies an action and returns a new game state. Does not modify the current state.
        Args:
            action: A tuple (from_row, from_col, to_row, to_col).
        Returns:
            A new ChessGame instance representing the state after the move.
        """
        from_row, from_col, to_row, to_col = action
        move = Move(from_row, from_col, to_row, to_col)
        new_board_ptr = self.board.step(move)
        if new_board_ptr:
            return ChessGame(board=new_board_ptr)
        return None

    def is_terminal(self):
        """
        Check if the game is over and return the outcome.
        Returns:
            The outcome (1 for White win, -1 for Black win, 0 for draw) if the game is over.
            None otherwise.
        """
        if self.board.is_game_over():
            return self.board.get_outcome()
        return None
    
    def reset(self):
        """
        Reset the chess board to the initial state.
        """
        self.board.reset()

    def copy(self):
        """
        Creates a deep copy of the current game state.
        Returns:
            A new ChessGame instance with a copied board state.
        """
        new_board_ptr = self.board.copy()
        return ChessGame(board=new_board_ptr)

    def outcome(self):
        """
        Get the outcome of the game as a string.
        Returns:
            A string representing the outcome: 'white', 'black', or 'draw'.
        """
        if not self.board.is_game_over():
            return None
        
        result = self.board.get_outcome()
        if result == 1:
            return 'white'
        elif result == -1:
            return 'black'
        else:
            return 'draw'

