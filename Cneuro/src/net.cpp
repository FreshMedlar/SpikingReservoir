#include "net.h"

// Multithreaded matrix-vector operations
Eigen::VectorXf SingleLayerNetwork::parallel_matvec(const Eigen::MatrixXf& W, 
                                                    const Eigen::VectorXf& x) {
    Eigen::VectorXf result(W.rows());
    omp_set_num_threads(12);
    #pragma omp parallel for
    for(int i = 0; i < W.rows(); ++i) {
        result[i] = W.row(i).dot(x);
    }
    return result;
}

Eigen::VectorXf SingleLayerNetwork::parallel_matvec_sparse(const Eigen::MatrixXf& W, 
                                                const std::vector<int>& indices) {
    Eigen::VectorXf result(W.rows());
    #pragma omp parallel for
    for(int i = 0; i < W.rows(); ++i) {
        float sum = 0.0f;
        for (int j : indices) {
            sum += W(i, j); // Sum weights at spike indices
        }
        result[i] = sum;
    }
    return result;
}

// Multithreaded outer product
Eigen::MatrixXf SingleLayerNetwork::parallel_outer_sparse(const Eigen::VectorXf& a, 
                                                const std::vector<int>& b) {
    Eigen::MatrixXf result = Eigen::MatrixXf::Zero(a.size(), size*10);
    #pragma omp parallel for
    for (int i : b) {
        result.col(i) = a;
    }
    return result;
}

Eigen::MatrixXf SingleLayerNetwork::parallel_outer(const Eigen::VectorXf& a,
                                                        const Eigen::VectorXf& b){
    Eigen::MatrixXf result = Eigen::MatrixXf::Zero(a.size(), b.size());
    #pragma omp parallel for
    for(int i = 0; i < a.size(); ++i) {
        result.row(i) = a[i] * b.transpose();
    }
    return result;
}

Eigen::VectorXf SingleLayerNetwork::softmax(const Eigen::VectorXf& logits) {
    Eigen::VectorXf exp_logits = (logits.array() - logits.maxCoeff()).exp();
    return exp_logits / exp_logits.sum();
}

SingleLayerNetwork::SingleLayerNetwork(float lr = 0.001, int size = 1000) : learning_rate(lr), size(size) {
    weights.setRandom(65, 10000);
    weights_transposed = weights.transpose(); 
    weights *= sqrtf(2.0f / size*10); 
    biases.setZero(65);
}

// Forward pass
// Eigen::VectorXf SingleLayerNetwork::forward(const Eigen::VectorXf& input) {
//     return parallel_matvec(weights, input) + biases;
// }

Eigen::VectorXf SingleLayerNetwork::forward_sparse(const std::vector<int>& spike_indices) {
    return parallel_matvec_sparse(weights, spike_indices) + biases;
}

Eigen::VectorXf SingleLayerNetwork::backward(const std::vector<int>& input, // 1x10000
                                            const Eigen::VectorXf& output, // 1x65
                                            int target) { // 1
    // Create one-hot encoded target vector
    Eigen::VectorXf one_hot = Eigen::VectorXf::Zero(output.size());
    one_hot(target) = 1.0f;

    // Compute output gradient (cross-entropy derivative)
    Eigen::VectorXf d_output = output - one_hot;

    // Compute gradients (multithreaded)
    // (65,1)x(1,10000)
    Eigen::MatrixXf d_weights = parallel_outer_sparse(d_output, input);
    Eigen::VectorXf d_biases = d_output;

    // Compute input gradient (multithreaded)
    Eigen::VectorXf d_input = parallel_matvec(weights_transposed, d_output);

    // Update weights and biases (in-place parallel updates)
    #pragma omp parallel for
    for(int i = 0; i < weights.rows(); ++i) {
        weights.row(i) -= learning_rate * d_weights.row(i);
        weights_transposed.col(i) = weights.row(i);
        biases[i] -= learning_rate * d_biases[i];
    }

    return d_input;
}

// Utility function to compute CrossEntropy loss
float SingleLayerNetwork::compute_loss(const Eigen::VectorXf& output, int target) {
    // Ensure output is after softmax (probabilities)
    float loss = 0.0f;
    loss -= std::log(output(target));  // Cross-entropy for the correct class
    return loss;
}

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
