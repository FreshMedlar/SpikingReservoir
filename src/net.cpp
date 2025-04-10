#include "net.h"

using namespace Eigen;

SingleLayerNetwork::SingleLayerNetwork(float lr, int size) : learning_rate(lr), size(size) {
    weights.setRandom(65, size);
    weights_transposed = weights.transpose(); 
    weights *= sqrtf(2.0f / size); 
    biases.setZero(65);
}

// Multithreaded matrix-vector operations
VectorXf SingleLayerNetwork::parallel_matvec(const MatrixXf& W, 
                                               const VectorXf& x) {
    VectorXf result(W.rows());
    omp_set_num_threads(12);
    #pragma omp parallel for
    for(int i = 0; i < W.rows(); ++i) {
        result[i] = W.row(i).dot(x);
    }
    return result;
}

VectorXf SingleLayerNetwork::parallel_matvec_sparse(const std::vector<short>& indices) {
    VectorXf result(weights.rows());
    #pragma omp parallel for
    for(int i = 0; i < weights.rows(); ++i) {
        for (int j : indices) {
            result[i] += weights(i, j); // Sum weights at spike indices
        }
    }
    return result;
}

// Multithreaded outer product
MatrixXf SingleLayerNetwork::parallel_outer_sparse(const VectorXf& a, 
                                                        const std::vector<short>& b) {
    MatrixXf result = MatrixXf::Zero(a.size(), size);
    #pragma omp parallel for
    for (int i : b) {
        result.col(i) = a;
    }
    return result;
}

MatrixXf SingleLayerNetwork::parallel_outer(const VectorXf& a,
                                                    const VectorXf& b){
    MatrixXf result = MatrixXf::Zero(a.size(), b.size());
    #pragma omp parallel for
    for(int i = 0; i < a.size(); ++i) {
        result.row(i) = a[i] * b.transpose();
    }
    return result;
}

VectorXf SingleLayerNetwork::softmax(const VectorXf& logits) {
    VectorXf exp_logits = (logits.array() - logits.maxCoeff()).exp();
    return exp_logits / exp_logits.sum();
}

VectorXf SingleLayerNetwork::forward_sparse(const std::vector<short>& spike_indices) {
    return parallel_matvec_sparse(spike_indices) + this->biases;
}

void SingleLayerNetwork::backward(const std::vector<short>& input, // 1x10000 spiked
                                            const VectorXf& output, // 1x65 softmax
                                            int target) { // 1
    // Create one-hot encoded target vector
    VectorXf one_hot = VectorXf::Zero(output.size());
    one_hot(target) = 1.0f;

    // Compute output gradient (cross-entropy derivative)
    VectorXf d_output = output - one_hot;

    // Compute gradients (multithreaded)
    // (65,1)x(1,10000)
    MatrixXf d_weights = parallel_outer_sparse(d_output, input);
    VectorXf d_biases = d_output;

    // Compute input gradient (multithreaded)
    // VectorXf d_input = parallel_matvec(weights_transposed, d_output);

    // Update weights and biases (in-place parallel updates)
    // #pragma omp parallel for
    for(int i = 0; i < weights.rows(); ++i) {
        weights.row(i) -= learning_rate * d_weights.row(i);
        weights_transposed.col(i) = weights.row(i);
        biases[i] -= learning_rate * d_biases[i];
    }

    // return d_input;
}

// Utility function to compute CrossEntropy loss
float SingleLayerNetwork::compute_loss(const VectorXf& output, int target) {
    float loss = 0.0f;
    loss -= std::log(output(target));  
    return loss;
}