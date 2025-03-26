#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <set>
#include "neuron.h"
#include <random>
#include <array>
#include "manager.h"
#include "scheduler.h"

extern std::vector<std::vector<float>> connectionMatrix;
extern std::vector<Neuron> neurons; // Declaration
extern std::random_device rd;
extern std::mt19937 gen;
extern float totalSum;
constexpr short SIZE = 1000;
extern std::vector<short> encodedTraining;

// refractory period
// extern const size_t COOLDOWN_FRAMES;
// extern std::array<std::vector<short>, 5> disableBuffer;
// extern int currentFrameIndex; // Tracks the current slot in the ring buffer

// buffer for spikes
extern const short SPIKE_FRAMES; 
extern const size_t SPIKE_BUFFER_SIZE;
extern std::array<std::vector<short>, 30> spikeBuffer;
extern int currentSpikeIndex;

// color
extern const size_t COLOR_FRAMES;
extern std::array<std::vector<short>, 5> colorBuffer;
extern int currentColorIndex; 

// manager and scheduler
extern Manager manager;
extern Scheduler scheduler;
extern float LR;
extern float TEMP;

// CONTROL VAR
// N of spikes in the last 500 steps
extern vector<int> spikeFreq;

#endif // GLOBALS_H