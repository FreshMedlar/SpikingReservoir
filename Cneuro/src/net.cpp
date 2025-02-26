#include "net.h"

using namespace torch;


// Net(int input, int output) : fc1_dims(input), out_dims(output) {
//     // Construct and register two Linear submodules.
//     fc1 = register_module("fc1", torch::nn::Linear(fc1_dims, out_dims));
// }

// // Implement the Net's algorithm.
// torch::Tensor forward(torch::Tensor x) {
//     // 1000x10
//     x = torch::relu(fc1->forward(x.reshape({1, fc1_dims})));
//     // x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
//     x = torch::log_softmax(x, /*dim=*/-1);
//     return x;
// }

// class SingleLayerNN(nn.Module):
//     def __init__(self, input_size, hidden_size ,output_size=65):
//         super(SingleLayerNN, self).__init__()
//         self.linear1 = nn.Linear(input_size, hidden_size)
//         self.linear2 = nn.Linear(hidden_size, output_size)
//         self.create_reservoir(input_size)
//         self.input_size = input_size
    
//     def reservoir_forward(self, input, targets=None):
//         xb_hot = tensor_to_one_hot(input, num_classes=self.input_size)
//         B, T, C = xb_hot.shape
//         output = torch.zeros(B, T, 65)

//         if T > 1:
//             for b in range(T - 1):
//                 xb_hot[:, b, :] = torch.atan(xb_hot[:, b, :] @ self.W_res)
//                 xb_hot[:, b + 1, :] += xb_hot[:, b, :]
//             xb_hot[:, T - 1, :] = torch.atan(xb_hot[:, T - 2, :] @ self.W_res)
//         else:
//             xb_hot[:, 0, :] = torch.atan(xb_hot[:, 0, :] @ self.W_res)

//         output, loss = self(xb_hot, targets)

//         return output, loss

//     def forward(self, x, targets=None): 
//         x = self.linear1(x)
//         x = torch.relu(x)
//         x = self.linear2(x)
//         output = torch.relu(x)

//         if targets == None:
//             loss = None
//         else:
//             B, T, C = output.shape
//             output = output.view(B*T, C) 
//             targets = targets.view(B*T)
//             loss = F.cross_entropy(output, targets.long())

//         return output, loss
    
//     def create_reservoir(self, N_res, eta=0.01):
//         def uniform_rvs(size):
//             return np.random.uniform(low=-1.0, high=1.0, size=size)
//         W = random(N_res, N_res, density=eta, format='csr', data_rvs=uniform_rvs)
//         W_dense = W.toarray()
//         #Calculate the max eigenvalue and find the max
//         max_eigenvalue = eigs(W, k=1, which='LM', return_eigenvectors=False)[0].real
//         # Normalize W to get W_res
//         rho = 0.9
//         W_res = rho * (W / max_eigenvalue)
//         W_res = torch.from_numpy(W_dense)
//         W_res = W_res.to(torch.float32)
//         self.W_res = W_res
//         return 0
    
//     def generate(self, idx, max_new_tokens):
//         # idx is (B, T) array of indices in the current context
//         for _ in range(max_new_tokens):
//             # get the predictions
//             logits, loss = self.reservoir_forward(idx)
//             # focus only on the last time step
//             logits = logits[:, -1, :] # becomes (B, C)
//             # apply softmax to get probabilities
//             probs = F.softmax(logits, dim=-1) # (B, C)
//             # sample from the distribution
//             idx_next = torch.multinomial(probs, num_samples=1) # (B, 1)
//             # append sampled index to the running sequence
//             idx = torch.cat((idx, idx_next), dim=1) # (B, T+1)
//         return idx
