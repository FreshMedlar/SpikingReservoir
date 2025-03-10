#include "scheduler.h"
#include "global.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>

Scheduler::Scheduler(int size) : size(size) {
}

void Scheduler::update() {
    // if (toAdd.size() > 0) {for (int n : toAdd) {for (Neuron* toadd : neurons[n].receivers){toadd->impulse(&(neurons[n]));}}}
    // update timeSinceSpike timer for all neurons
    for (Neuron& neuron : neurons) {
        neuron.timeSinceSpike += 1;
    } 
    // check if neurons have spiked
    if (toSpike.size() > 0) {
        for (int n : toSpike) {
            spike(neurons[n]);
        }
        // std::cout << "here?" << std::endl;
        toSpike.clear();
        std::swap(toSpike, swapSpike);
        swapSpike.clear();
        // std::cout << "clean" << std::endl;
    }
}

// void Scheduler::synaptoGenesis() {
//     if (lonelyNeurons.size() != 0) {
//         for (int toconnect : lonelyNeurons) {
//             drawNeuron().connect(toconnect); 
//         }
//         lonelyNeurons.clear();
//     }
// }

Neuron& Scheduler::drawNeuron() {
    // Calculate the total friendliness
    int totalreceivers = std::accumulate(neurons.begin(), neurons.end(), 0,
                                            [](int sum, const Neuron& neuron) {
                                                return sum + receivers[neuron.ID].size();
                                            });

    // Generate a random number between 0 and totalFriendliness
    std::uniform_int_distribution<> dis(0, totalreceivers - 1);
    int randomNumber = dis(gen);

    // Select a neuron based on the random number
    for (auto& neuron : neurons) {
        if (randomNumber < receivers[neuron.ID].size()+2) {
            return neuron;
        }
        randomNumber -= receivers[neuron.ID].size();
    }

    // In case of rounding errors or empty vector, return the last neuron
    return neurons.back();
}
