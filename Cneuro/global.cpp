#include "global.h"

std::vector<std::vector<float>> connectionMatrix;
std::vector<Neuron> neurons; // Definition
std::random_device rd;
std::mt19937 gen(rd());
