#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "neuron.h" // Assuming Neuron is defined in Neuron.h
#include <random>

extern std::vector<std::vector<float>> connectionMatrix;
extern std::vector<Neuron> neurons; // Declaration
extern std::random_device rd;
extern std::mt19937 gen;

#endif // GLOBALS_H