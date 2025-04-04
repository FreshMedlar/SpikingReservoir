#ifndef SINGLELAYERNETWORK_H
#define SINGLELAYERNETWORK_H

#include <iostream>
#include <eigen3/Eigen/Dense>
#include <omp.h>
#include <cmath>
#include <set>

class SingleLayerNetwork {
private:
    Eigen::MatrixXf weights;  // (65, 10000)
    Eigen::MatrixXf weights_transposed; // (10000,65)
    Eigen::VectorXf biases;   // (65, 1)
    float learning_rate;
    int size;

    Eigen::VectorXf parallel_matvec(const Eigen::MatrixXf& W,
                                    const Eigen::VectorXf& x);
    Eigen::VectorXf parallel_matvec_sparse(const Eigen::MatrixXf& W, 
                                        const std::vector<short>& indices);
    Eigen::MatrixXf parallel_outer_sparse( const Eigen::VectorXf& a, 
                                    const std::vector<short>& b);
    Eigen::MatrixXf parallel_outer(const Eigen::VectorXf& a,
                                        const Eigen::VectorXf& b);


public:
    SingleLayerNetwork(float lr, int size);
    Eigen::VectorXf softmax(const Eigen::VectorXf& logits);
    Eigen::VectorXf forward(const Eigen::VectorXf& input);
    Eigen::VectorXf forward_sparse(const std::vector<short>& spike_indices);
    Eigen::VectorXf backward(const std::vector<short>& input, 
                            const Eigen::VectorXf& output,
                            int target);
    float compute_loss(const Eigen::VectorXf& output,
                        int target); 
};

#endif // SINGLELAYERNETWORK_H

