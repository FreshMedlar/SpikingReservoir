#ifndef SINGLELAYERNETWORK_H
#define SINGLELAYERNETWORK_H

#include <iostream>
#include <Eigen/Dense>
#include <omp.h>
#include <cmath>

class SingleLayerNetwork {
private:
    Eigen::MatrixXf weights;  // (65, 10000)
    Eigen::VectorXf biases;   // (65, 1)
    float learning_rate;

    Eigen::VectorXf parallel_matvec(const Eigen::MatrixXf& W,
                                    const Eigen::VectorXf& x);
    Eigen::MatrixXf parallel_outer( const Eigen::VectorXf& a, 
                                    const Eigen::VectorXf& b);


public:
    SingleLayerNetwork(float lr);
    Eigen::VectorXf softmax(const Eigen::VectorXf& logits);
    Eigen::VectorXf forward(const Eigen::VectorXf& input);
    Eigen::VectorXf backward(const Eigen::VectorXf& input, 
                            const Eigen::VectorXf& output,
                            const Eigen::VectorXf& target); 
    static float compute_loss(const Eigen::VectorXf& output, 
                            int target);
};

#endif // SINGLELAYERNETWORK_H