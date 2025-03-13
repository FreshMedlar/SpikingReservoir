#include "global.h"

// row n is for outgoing connection of neuron n 
std::vector<std::vector<float>> connectionMatrix;
std::vector<Neuron> neurons; 
std::random_device rd;
std::mt19937 gen(rd());
float totalSum = 0.0f;

// buffer for refractory period
const size_t COOLDOWN_FRAMES = 7;
std::array<std::vector<short>, COOLDOWN_FRAMES> disableBuffer;
int currentFrameIndex = 0; // Tracks the current slot in the ring buffer

// buffer for spikes
const short SPIKE_FRAMES = 2;
const size_t SPIKE_BUFFER_SIZE = 1000;
std::array<std::set<short>, SPIKE_BUFFER_SIZE> spikeBuffer;
int currentSpikeIndex = 0;

//
Manager manager(SIZE);
Scheduler scheduler(SIZE);
float LR = 1;
float TEMP = 3; // sensibility to timing