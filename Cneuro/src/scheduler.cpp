#include "scheduler.h"
#include "global.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <omp.h>

Scheduler::Scheduler(int size) : size(size) {
}

void Scheduler::update() {
    // update timeSinceSpike timer for all neurons
    for (int neu = 0; neu < SIZE; neu++) {
        timeSinceSpike[neu] += 1;
        // excitability[neu] += excitability[neu]*(0.1f * (1.0 - excitability[neu]));

    } 
    for (short neur : spikeBuffer[currentSpikeIndex]) { spike(neur); 
                                                        active[neur] = true;}
    spikeBuffer[currentSpikeIndex].clear(); // Reset the slot
    currentSpikeIndex = (currentSpikeIndex + 1) % SPIKE_BUFFER_SIZE;
}

void Scheduler::updateColor() {
    for (short obj : colorBuffer[currentColorIndex]) {
        colors[obj] = WHITE;
    }
    colorBuffer[currentColorIndex].clear(); // Reset the slot
    // Advance the ring buffer index
    currentColorIndex = (currentColorIndex + 1) % COLOR_FRAMES;
}

void Scheduler::pruningAndDecay() {
    // #pragma omp parallel for
    for (size_t row = 0; row < SIZE; row++) {
        if (biases[row] < 0.1) {
            for (size_t col = 0; col < SIZE; col++) {
                if (connectionMatrix[col][row] < 0.1f){
                    disconnect(col, row);
                }
                connectionMatrix[row][col] -= 0.001f;
            }
        } 
        biases[row] -= 0.001f;
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
