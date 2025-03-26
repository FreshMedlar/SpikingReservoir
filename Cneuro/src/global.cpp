#include "global.h"

// row n is for outgoing connection of neuron n 
std::vector<std::vector<float>> connectionMatrix;
std::vector<Neuron> neurons; 
std::random_device rd;
std::mt19937 gen(rd());
float totalSum = 0.0f;
vector<short> encodedTraining;

// buffer for refractory period
// const size_t COOLDOWN_FRAMES = 5;
// std::array<std::vector<short>, COOLDOWN_FRAMES> disableBuffer;
// int currentFrameIndex = 0; // Tracks the current slot in the ring buffer

// buffer for spikes
const short SPIKE_FRAMES = 3;
const size_t SPIKE_BUFFER_SIZE = 30;
std::array<std::vector<short>, SPIKE_BUFFER_SIZE> spikeBuffer;
int currentSpikeIndex = 0;

// color buffer
const size_t COLOR_FRAMES = 5;
std::array<std::vector<short>, COLOR_FRAMES> colorBuffer;
int currentColorIndex = 0; 

//
Manager manager(SIZE);
Scheduler scheduler(SIZE);
float LR = 1; // magnitued of weights updated
float TEMP = 3; // importance of timing (low TEMP, strict timing)

// CONTROL VAR
vector<int> spikeFreq(500, 0);