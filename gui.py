import pygame
import sys
import os
import io
from contextlib import redirect_stdout
import chessengine

# Colors
DARK_SQUARE = (118, 150, 86)    # Green
LIGHT_SQUARE = (238, 238, 210)  # Cream
HIGHLIGHT = (186, 202, 68)      # Light green highlight
VALID_MOVE = (255, 255, 0, 128) # Semi-transparent yellow

# Board dimensions
WIDTH, HEIGHT = 800, 800
ROWS, COLS = 8, 8
SQUARE_SIZE = WIDTH // COLS

class ChessGUI:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        pygame.display.set_caption('Chess Game')
        
        self.board = chessengine.ChessBoard()
        self.selected_square = None
        self.valid_moves = []
        self.piece_images = self.load_images()
        
    def load_images(self):
        """Load chess piece images"""
        pieces = {}
        piece_chars = {'p': 'pawn', 'r': 'rook', 'n': 'knight', 
                      'b': 'bishop', 'q': 'queen', 'k': 'king'}
        
        for color in ['w', 'b']:
            for code, name in piece_chars.items():
                filename = f"{color}{code}.png"
                try:
                    image = pygame.image.load(os.path.join('images', filename))
                    pieces[f"{color}{code}"] = pygame.transform.scale(
                        image, (SQUARE_SIZE, SQUARE_SIZE))
                except:
                    print(f"Error loading {filename}. Make sure it exists in the images folder.")
        return pieces
        
    def get_board_state(self):
        """Get board state directly from C++ engine"""
        return self.board.get_board_state_chars()
    
    def draw_board(self):
        """Draw the chess board squares"""
        for row in range(ROWS):
            for col in range(COLS):
                color = LIGHT_SQUARE if (row + col) % 2 == 0 else DARK_SQUARE
                pygame.draw.rect(self.screen, color, 
                                (col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE))
    
    def draw_pieces(self, board_state):
        """Draw pieces on the board"""
        for row in range(len(board_state)):
            for col in range(len(board_state[row])):
                piece = board_state[row][col]
                if piece != '.':
                    # Map FEN character to piece image key
                    color = 'w' if piece.isupper() else 'b'
                    piece_type = piece.lower()
                    key = f'{color}{piece_type}'
                    
                    if key in self.piece_images:
                        self.screen.blit(self.piece_images[key], 
                                        (col * SQUARE_SIZE, row * SQUARE_SIZE))
    
    def highlight_squares(self):
        """Highlight selected square and valid moves"""
        # Highlight valid moves
        for move_row, move_col in self.valid_moves:
            highlight_surf = pygame.Surface((SQUARE_SIZE, SQUARE_SIZE), pygame.SRCALPHA)
            highlight_surf.fill(VALID_MOVE)
            self.screen.blit(highlight_surf, (move_col * SQUARE_SIZE, move_row * SQUARE_SIZE))
        
        # Highlight selected square
        if self.selected_square:
            row, col = self.selected_square
            pygame.draw.rect(self.screen, HIGHLIGHT, 
                            (col * SQUARE_SIZE, row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE), 4)
    
    def find_valid_moves(self, row, col):
        """Find valid moves for a piece at the given position"""
        valid_destinations = []
        
        # Get all valid moves from the engine
        all_valid_moves = self.board.get_valid_moves()
        
        # Filter for moves that start from our selected square
        for move in all_valid_moves:
            if move.start.x == row and move.start.y == col:
                valid_destinations.append((move.to.x, move.to.y))
        
        return valid_destinations
    
    def handle_click(self, row, col):
        """Handle mouse click on the board"""
        if self.selected_square:
            # If a square is already selected, try to make a move
            from_row, from_col = self.selected_square
            
            if (row, col) in self.valid_moves:
                # This is a valid move
                start_coords = chessengine.Coords(from_row, from_col)
                to_coords = chessengine.Coords(row, col)
                move = chessengine.Move(start_coords, to_coords)
                
                if self.board.make_move(move):
                    self.selected_square = None
                    self.valid_moves = []
            else:
                # Not a valid move, select this square instead
                self.selected_square = (row, col)
                self.valid_moves = self.find_valid_moves(row, col)
        else:
            # Select this square
            self.selected_square = (row, col)
            self.valid_moves = self.find_valid_moves(row, col)
    
    def run(self):
        """Main game loop"""
        running = True
        while running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False
                
                if event.type == pygame.MOUSEBUTTONDOWN:
                    col = event.pos[0] // SQUARE_SIZE
                    row = event.pos[1] // SQUARE_SIZE
                    self.handle_click(row, col)
            if self.board.is_game_over():
                print("Game Over!")
                running = False
            
            # Draw everything
            self.draw_board()
            self.highlight_squares()
            self.draw_pieces(self.get_board_state())
            
            pygame.display.flip()
        
        pygame.quit()
        sys.exit()

if __name__ == "__main__":
    gui = ChessGUI()
    gui.run()