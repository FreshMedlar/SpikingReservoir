#include "scheduler.h"
#include "global.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>

Scheduler::Scheduler(int size) : size(size) {
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
    int totalReceiver = std::accumulate(neurons.begin(), neurons.end(), 0,
                                            [](int sum, const Neuron& neuron) {
                                                return sum + neuron.receiver.size();
                                            });

    // Generate a random number between 0 and totalFriendliness
    std::uniform_int_distribution<> dis(0, totalReceiver - 1);
    int randomNumber = dis(gen);

    // Select a neuron based on the random number
    for (auto& neuron : neurons) {
        if (randomNumber < neuron.receiver.size()+2) {
            return neuron;
        }
        randomNumber -= neuron.receiver.size();
    }

    // In case of rounding errors or empty vector, return the last neuron
    return neurons.back();
}
