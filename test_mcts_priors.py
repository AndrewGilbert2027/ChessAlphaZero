"""Tests for PUCT action selection in MCTS.py.

These use small stand-in state and model objects rather than the C++ engine, so
the search logic can be tested without compiling the bindings.
"""
from MCTS import Deep_Node


class StubState:
    """A one-ply game: every action leads to a non-terminal state for the opponent."""

    def __init__(self, player=1, depth=0):
        self.player = player
        self.depth = depth

    def copy(self):
        return StubState(self.player, self.depth)

    def is_terminal(self):
        return None

    def step(self, action):
        return StubState(-self.player, self.depth + 1)


class StubModel:
    """Returns a fixed value and a policy that strongly prefers one action."""

    def __init__(self, policy):
        self.policy = policy

    def forward(self, state):
        return 0.0, dict(self.policy)


def test_first_selection_follows_the_policy_prior():
    """
    On a node's first visit no action has been tried, so the prior is the only
    information available and PUCT should pick the action the network likes.

    This previously failed: the total visit count is zero on the first visit, so
    the exploration term carried a sqrt(0) and every action scored 0.0, making
    the choice fall out of dictionary order instead of the policy.
    """
    node = Deep_Node(StubState(), StubModel({"a": 0.01, "b": 0.97, "c": 0.02}))

    assert node._select_action(puct=1.0) == "b"


def test_prior_still_decides_when_the_favoured_action_is_last():
    node = Deep_Node(StubState(), StubModel({"a": 0.02, "b": 0.03, "c": 0.95}))

    assert node._select_action(puct=1.0) == "c"


def test_visited_actions_trade_off_value_against_the_prior():
    """
    Once a node has visits, a good Q should be able to outweigh a better prior.

    Q is stored from the perspective of the player to move in the child, and T
    carries the sign flip, so a move that is good for us is one whose child Q is
    negative.
    """
    node = Deep_Node(StubState(), StubModel({"a": 0.1, "b": 0.9}))

    # "a" was tried once and left the opponent in a losing position.
    node.N["a"] = 1
    node.Q["a"] = -10.0
    node.N_visits = 1

    assert node._select_action(puct=1.0) == "a"


def test_exploration_term_damps_a_heavily_visited_action():
    """At equal Q and equal prior, the less-visited action is preferred."""
    node = Deep_Node(StubState(), StubModel({"a": 0.5, "b": 0.5}))

    node.N["a"] = 50
    node.N["b"] = 0
    node.N_visits = 50

    assert node._select_action(puct=1.0) == "b"
