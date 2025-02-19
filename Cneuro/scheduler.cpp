#include "scheduler.h"
#include "global.h"

Scheduler::Scheduler(int size) : size(size) {
    // toReceive.clear();
    // toReceive.reserve(size);  // Reserve memory to optimize performance
    // for (int i = 0; i < size; i++) {
    //     toReceive[i] = &(neurons[i].receiver);  // Create Neuron with ID = i
    // }
}

void Scheduler::update() {
    // if (toAdd.size() > 0) {for (int n : toAdd) {for (Neuron* toadd : neurons[n].receiver){toadd->impulse(&(neurons[n]));}}}
    // update timeSinceSpike timer for all neurons
    for (Neuron& neuron : neurons) {
        neuron.timeSinceSpike += 1;
    } 
    // check if neurons have spiked
    if (toSpike.size() > 0) {
        for (int n : toSpike) {
            neurons[n].spike(nullptr);
        }
        toSpike.clear();
    }
}

void Scheduler::changeColor() {
    if (!trackColor.empty()) {
        trackColor.erase(
            std::remove_if(trackColor.begin(), trackColor.end(),
                [this](std::pair<int, int>& n) {
                    if (n.second == 0) {
                        neurons[n.first].color = WHITE;
                        return true;  // Mark for removal
                    }
                    n.second--;  // Decrement countdown
                    return false;
                }),
            trackColor.end()
        );
    }
}