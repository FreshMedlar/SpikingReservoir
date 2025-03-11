#include "global.h"

// row n is for outgoing connection of neuron n 
std::vector<std::vector<float>> connectionMatrix;
std::vector<Neuron> neurons; 
std::random_device rd;
std::mt19937 gen(rd());
float totalSum = 0.0f;

// buffer for refractory period
const size_t COOLDOWN_FRAMES = 50;
std::array<std::vector<Neuron*>, COOLDOWN_FRAMES> disableBuffer;
int currentFrameIndex = 0; // Tracks the current slot in the ring buffer

//
Manager manager(SIZE);
Scheduler scheduler(SIZE);
