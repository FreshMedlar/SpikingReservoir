#include "scheduler.h"
#include "global.h"
#include <iostream>
#include <algorithm>
#include <numeric>

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
    Neuron neu = drawNeuron();
    for (Neuron* toconnect : lonelyNeurons) {
        neu.connect(toconnect); // TODO should be the opposite
    }
    lonelyNeurons.clear();
}

Neuron Scheduler::drawNeuron() {
    // Calculate the total friendliness
    int totalFriendliness = std::accumulate(neurons.begin(), neurons.end(), 0,
                                            [](int sum, const Neuron& neuron) {
                                                return sum + neuron.friendliness;
                                            });

    // Generate a random number between 0 and totalFriendliness
    int randomNumber = rand() % totalFriendliness;

    // Select a neuron based on the random number
    for (const auto& neuron : neurons) {
        if (randomNumber < neuron.friendliness) {
            return neuron;
        }
        randomNumber -= neuron.friendliness;
    }

    // In case of rounding errors or empty vector, return the last neuron
    return neurons.back();
}