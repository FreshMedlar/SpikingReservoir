#include "global.h"

std::vector<std::vector<int>> connectionMatrix;
std::vector<Neuron> neurons; // Definition
std::random_device rd;
std::mt19937 gen(rd());