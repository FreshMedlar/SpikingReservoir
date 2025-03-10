#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "neuron.h"
#include <random>
#include <array>
#include "manager.h"
#include "scheduler.h"

extern std::vector<std::vector<float>> connectionMatrix;
extern std::vector<Neuron> neurons; // Declaration
extern std::random_device rd;
extern std::mt19937 gen;
constexpr short SIZE = 1000;

// refractory period
extern const size_t COOLDOWN_FRAMES;
extern std::array<std::vector<Neuron*>, 100> disableBuffer;
extern int currentFrameIndex; // Tracks the current slot in the ring buffer

// manager and scheduler
extern Manager manager;
extern Scheduler scheduler;

#endif // GLOBALS_H