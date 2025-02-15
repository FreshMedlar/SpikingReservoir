#include "scheduler.h"

extern std::vector<Neuron> neurons;

Scheduler::Scheduler() {
    toReceive.clear();
    toReceive.reserve(size);  // Reserve memory to optimize performance
    for (int i = 0; i < size; i++) {
        toReceive.emplace_back(i, nullptr);  // Create Neuron with ID = i
    }
}

