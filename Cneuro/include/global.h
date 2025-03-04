#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "neuron.h"
#include <random>
#include <array>

extern std::vector<std::vector<float>> connectionMatrix;
extern std::vector<Neuron> neurons; // Declaration
extern std::random_device rd;
extern std::mt19937 gen;
extern int SIZE;

// refractory period
extern const size_t COOLDOWN_FRAMES;
extern std::array<std::vector<Neuron*>, 4> disableBuffer;
extern int currentFrameIndex; // Tracks the current slot in the ring buffer


#endif // GLOBALS_H