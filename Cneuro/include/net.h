#include <iostream>
#include <torch/torch.h>

struct NetImpl : torch::nn::Module {
    NetImpl(int fc1_dims, int out_dims) :
        fc1(register_module("fc1", torch::nn::Linear(fc1_dims, out_dims))) {
    }

    torch::Tensor forward(torch::Tensor x) {
        x = torch::relu(fc1(x));
        return x;
    }

    torch::nn::Linear fc1{nullptr}, out{nullptr};
};

TORCH_MODULE(Net);