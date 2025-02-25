#include "scheduler.h"
#include "global.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>

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
    if (lonelyNeurons.size() != 0) {
        for (Neuron* toconnect : lonelyNeurons) {
            drawNeuron().connect(toconnect); 
        }
        lonelyNeurons.clear();
    }
}

Neuron& Scheduler::drawNeuron() {
    // Calculate the total friendliness
    int totalFriendliness = std::accumulate(neurons.begin(), neurons.end(), 0,
                                            [](int sum, const Neuron& neuron) {
                                                return sum + neuron.friendliness;
                                            });

    // Generate a random number between 0 and totalFriendliness
    std::uniform_int_distribution<> dis(0, totalFriendliness - 1);
    int randomNumber = dis(gen);

    // Select a neuron based on the random number
    for (auto& neuron : neurons) {
        if (randomNumber < neuron.friendliness) {
            return neuron;
        }
        randomNumber -= neuron.friendliness;
    }

    // In case of rounding errors or empty vector, return the last neuron
    return neurons.back();
}