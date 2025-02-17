#include "scheduler.h"
#include "global.h"

Scheduler::Scheduler(int size) : size(size) {
    toReceive.clear();
    toReceive.reserve(size);  // Reserve memory to optimize performance
    for (int i = 0; i < size; i++) {
        toReceive[i] = &(neurons[i].receiver);  // Create Neuron with ID = i
    }
}

void Scheduler::update() {
    // if (toAdd.size() > 0) {for (int n : toAdd) {for (Neuron* toadd : neurons[n].receiver){toadd->impulse(&(neurons[n]));}}}
    if (toSpike.size() > 0) {
        for (int n : toSpike) {
            neurons[n].spike(nullptr);
        }
        toSpike.clear();
    }
}