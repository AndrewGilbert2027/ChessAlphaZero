from Game import ChessGame
from Model import ChessCNN
from MCTS import MCTS_Deep
from time import time

def main():
    print("Initializing Chess Game and Model...")
    game = ChessGame()
    model = ChessCNN()
    print("Chess Game and Model initialized.")


    print("Initializing MCTS...")
    mcts = MCTS_Deep(game, model)
    print("MCTS initialized.")
    print("Starting MCTS search...")
    start_time = time()
    est_val, policy_dict = mcts.search(1000)
    end_time = time()
    print(f"MCTS search completed in {end_time - start_time:.2f} seconds.")
    print("Finished MCTS search.")
    print(f"Estimated Value: {est_val}")
    print("Policy Dictionary:")
    for move, prob in policy_dict.items():
        print(f"Move {move}: Probability {(prob / 1000):.4f}")
    print("MCTS search completed.")
    print("Test done.")

if __name__ == "__main__":
    main()