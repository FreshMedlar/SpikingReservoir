#include "net.h"

SingleLayerNetwork::SingleLayerNetwork(float lr, int size) : learning_rate(lr), size(size) {
    weights.setRandom(65, size*10);
    weights_transposed = weights.transpose(); 
    weights *= sqrtf(2.0f / size*10); 
    biases.setZero(65);
}

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
    float loss = 0.0f;
    loss -= std::log(output(target));  
    return loss;
}