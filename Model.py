import torch
import torch.nn as nn
import torch.nn.functional as F
import numpy as np

class ChessCNN(nn.Module):
    def __init__(self, input_channels=9, num_channels=128):
        """Initialize the Chess CNN model."""
        super(ChessCNN, self).__init__()
        
        # Shared convolutional layers
        self.conv1 = nn.Conv2d(input_channels, num_channels, kernel_size=3, padding=1)
        self.bn1 = nn.BatchNorm2d(num_channels)
        
        self.conv2 = nn.Conv2d(num_channels, num_channels, kernel_size=3, padding=1)
        self.bn2 = nn.BatchNorm2d(num_channels)
        
        self.conv3 = nn.Conv2d(num_channels, num_channels, kernel_size=3, padding=1)
        self.bn3 = nn.BatchNorm2d(num_channels)

        # Policy head
        self.policy_conv = nn.Conv2d(num_channels, 4, kernel_size=1)
        self.policy_bn = nn.BatchNorm2d(4)
        self.policy_fc = nn.Linear(4 * 8 * 8, 8 * 8 * 8 * 8)

        # Value head
        self.value_conv = nn.Conv2d(num_channels, 2, kernel_size=1)
        self.value_bn = nn.BatchNorm2d(2)
        self.value_fc1 = nn.Linear(2 * 8 * 8, 128)
        self.value_fc2 = nn.Linear(128, 1)

        self.policy_shape = (8, 8, 8, 8)
        self.value_shape = (1,)

    def forward(self, state):
        """
        Forward pass through the network.
        
        Args:
            state: Game state object with get_feature_plane and get_policy_mask methods or a tensor.
        
        Returns:
            value: Scalar value estimate (-1 to 1).
            policy: Dictionary mapping valid move tuples to probabilities.
        """
        if isinstance(state, torch.Tensor):
            x = self._prepare_tensor(state)
            # Cannot infer valid moves from tensor alone, this path is for get_value or direct network calls
            policy_mask = None 
        else:
            try:
                feature_plane = state.get_feature_plane()
                policy_mask = state.get_policy_mask()
                x = self._prepare_tensor(feature_plane)
            except AttributeError:
                raise ValueError("Input state must have get_feature_plane and get_policy_mask methods")

        value, policy_logits = self._network_forward(x)
        
        if policy_mask is not None:
            policy_dict = self._create_policy_dict(policy_logits, policy_mask)
            return value.item(), policy_dict
        else:
            # If no mask, return raw logits (used internally by get_value)
            return value, policy_logits

    def _prepare_tensor(self, tensor):
        """Prepare input tensor for the network."""
        if not isinstance(tensor, torch.Tensor):
            tensor = torch.FloatTensor(tensor)
        
        if tensor.dim() == 4: # Already (batch, channels, height, width)
            return tensor
        if tensor.dim() == 3: # (channels, height, width)
            return tensor.unsqueeze(0)
        
        raise ValueError(f"Unexpected tensor shape: {tensor.shape}. Must be 3D or 4D.")

    def _network_forward(self, x):
        """Internal network forward pass."""
        # Shared representation
        x = F.relu(self.bn1(self.conv1(x)))
        x = F.relu(self.bn2(self.conv2(x)))
        x = F.relu(self.bn3(self.conv3(x)))
        
        # Policy head
        policy = F.relu(self.policy_bn(self.policy_conv(x)))
        policy = policy.view(x.size(0), -1)
        policy = self.policy_fc(policy)
        
        # Value head
        value = F.relu(self.value_bn(self.value_conv(x)))
        value = value.view(x.size(0), -1)
        value = F.relu(self.value_fc1(value))
        value = torch.tanh(self.value_fc2(value))
        
        return value, policy

    def _create_policy_dict(self, policy_logits, policy_mask):
        """Convert policy logits to a dictionary of probabilities for valid moves."""
        policy_probs = F.softmax(policy_logits, dim=1)
        
        mask = torch.FloatTensor(policy_mask).view(1, -1)
        
        masked_policy = policy_probs * mask
        total_prob = masked_policy.sum()

        if total_prob > 1e-9:
            masked_policy /= total_prob
        else:
            # If all valid moves have zero probability, assign uniform probability
            num_valid_moves = int(mask.sum().item())
            if num_valid_moves > 0:
                masked_policy = mask / num_valid_moves
        
        policy_dict = {}
        non_zero_indices = torch.nonzero(masked_policy.squeeze(), as_tuple=False).squeeze()
        
        for idx in non_zero_indices:
            i = idx.item()
            prob = masked_policy[0, i].item()
            
            # Convert flat index back to 4D move coordinates
            to_y = i % 8
            i //= 8
            to_x = i % 8
            i //= 8
            from_y = i % 8
            i //= 8
            from_x = i % 8
            
            policy_dict[(from_x, from_y, to_x, to_y)] = prob
            
        return policy_dict
    
    def get_value(self, state):
        """Get value evaluation for a state."""
        with torch.no_grad():
            if isinstance(state, torch.Tensor):
                x = self._prepare_tensor(state)
            else:
                try:
                    feature_plane = state.get_feature_plane()
                    x = self._prepare_tensor(torch.FloatTensor(feature_plane))
                except AttributeError:
                    raise ValueError("Input state must have get_feature_plane method")
            
            value, _ = self._network_forward(x)
            return value.item()

    def save(self, path):
        """Save the model to a file."""
        torch.save(self.state_dict(), path)

    def load(self, path):
        """Load the model from a file."""
        self.load_state_dict(torch.load(path))
        self.eval()