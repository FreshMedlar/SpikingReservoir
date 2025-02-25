import numpy as np
import torch








with open('tinyshakespeare.txt', 'r', encoding='utf-8') as f:
    text = f.read()


chars = sorted(list(set(text)))
vocab_size = len(chars)

# create a mapping from characters to integers
stoi = { ch:i for i,ch in enumerate(chars) }
itos = { i:ch for i,ch in enumerate(chars) }
encode = lambda s: [stoi[c] for c in s] # encoder: take a string, output a list of integers
decode = lambda l: ''.join([itos[i] for i in l]) # decoder: take a list of integers, output a string

# let's now encode the entire text dataset and store it into a torch.Tensor
data = torch.tensor(encode(text), dtype=torch.long)

# Let's now split up the data into train and validation sets
n = int(0.9*len(data)) # first 90% will be train, rest val
train_data = data[:n]
val_data = data[n:]


def generate_spike_trains(input_data, U_max, U_min, N_sample):
    """
    Generate spike trains using Poisson encoding.
    
    Parameters:
        input_data (numpy.ndarray): Normalized input data (e.g., between U_min and U_max).
        U_max (float): Maximum value of the input range.
        U_min (float): Minimum value of the input range.
        N_sample (int): Number of spike samples.
    
    Returns:
        spike_trains (numpy.ndarray): Generated spike trains (0s and 1s).
    """
    # Calculate average spike interval
    h_k = N_sample * (U_max - input_data) / (U_max - U_min)
    
    # Generate intervals using Poisson distribution
    intervals = np.random.poisson(h_k)
    
    # Generate spike trains
    spike_trains = np.zeros((len(input_data), N_sample))
    for i, interval in enumerate(intervals):
        spike_indices = np.cumsum(np.random.choice(np.arange(1, N_sample + 1), interval, replace=False))
        spike_indices = spike_indices[spike_indices < N_sample]
        spike_trains[i, spike_indices] = 111
    
    return spike_trains

def char_to_spike_train(char):
    """
    One hot encoding of a character
    
    Parameters: 
        char (string): the character to encode
    
    Returns:
        np.array of dimensions (N_sam, 65)   
    """
    N_sam = 10
    spikes = np.zeros((N_sam, 65))
    for i in range(N_sam):
        spikes[i, encode(char)] = 1

    return spikes

