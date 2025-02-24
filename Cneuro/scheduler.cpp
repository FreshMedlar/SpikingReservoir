#include "scheduler.h"
#include "global.h"
#include <iostream>
#include <algorithm>

Scheduler::Scheduler(int size) : size(size) {}

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
        // std::cout << "here?" << std::endl;
        toSpike.clear();
        std::swap(toSpike, nextSpike);

        // std::cout << "clean" << std::endl;
    }
}

void Scheduler::changeColor() {
    if (!trackColor.empty()) {
        for (auto it = trackColor.begin(); it != trackColor.end();) {
            if (it->second == 0) {
                if (it->first < neurons.size()) {
                    neurons[it->first].color = WHITE;
                }
                it = trackColor.erase(it);  // Safe erase-while-iterating
            } else {
                --(it->second);
                ++it;
            }
        }
    }
}

void Scheduler::synaptoGenesis() {
    for (Neuron* nen : lonelyNeurons) {
        nen->connect(nullptr);
    }
    lonelyNeurons.clear();
}