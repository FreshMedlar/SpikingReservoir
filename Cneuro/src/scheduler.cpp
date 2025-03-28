#include "scheduler.h"
#include "global.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <omp.h>

#include "raylib.h"

Scheduler::Scheduler(int size) : size(size) {
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
            
            
void Scheduler::update() {
    // update timeSinceSpike timer for all neurons
    for (int neu = 0; neu < SIZE; neu++) {
        timeSinceSpike[neu] += 1;
        excitability[neu] -= 0.06f * (1.0f - excitability[neu]);
        // actionPotential[neu] -= 0.08f * excitability[neu];

    } 
    for (short neur : spikeBuffer[currentSpikeIndex]) { spike(neur); } 
                                                        //active[neur] = true;}
    spikeBuffer[currentSpikeIndex].clear(); // Reset the slot
    currentSpikeIndex = (currentSpikeIndex + 1) % SPIKE_BUFFER_SIZE;
}

void Scheduler::step (short letter) {
    // INPUT
    // short input = SIZE - letter;
    // if (active[input] == true) {
    //     spikeBuffer[currentSpikeIndex].push_back(input);
    //     active[input] = false;
    // }
    //RESERVOIR
    // spikeFreq[letter%100] = spikeBuffer[currentSpikeIndex].size();
    totalSum += spikeBuffer[currentSpikeIndex].size();
    scheduler.update();
}
                
float Scheduler::simulation() { //, vector<int>& connectionsPerNeuron) {
    bool draw = false;
    for (int letter = 0; letter < 1000; letter++) { //encodedTraining.size()-1
        for (int cycle = 0; cycle < 10; cycle++) {
            if (draw) {
                BeginDrawing();
                ClearBackground(BLACK);
                
                //DRAW
                // manager.draw();
                // manager.applyForces();
                //GRAPH
                // connectionsPerNeuron.clear();
                // connectionsPerNeuron.resize(SIZE, 0); 
                // EITHER, NOT BOTH
                // manager.receiversFrequence(connectionsPerNeuron.data()); 
                // manager.sendersFrequence(connectionsPerNeuron.data());
                // manager.drawreceiversGraph(connectionsPerNeuron); // Draw the plot
                // manager.clustering();
                // SPIKES
                manager.drawSpikesGraph(spikeFreq);
                // totalWeight[(epoch)%500] = totalSum;
                manager.drawTotalWeight();
                manager.drawOrder();
                manager.drawChaos();
                std::vector<float> sorted = frequency;  // Copy the original vector
                std::sort(sorted.begin(), sorted.end());  // Sort in ascending order
                manager.drawSpikeFrequencyDistribution(sorted);
                
                // FPS  
                int fps = GetFPS();
                DrawText(TextFormat("FPS: %d", fps), 10, 10, 20, GREEN); 
                EndDrawing();
                scheduler.updateColor();
            }
            
            scheduler.step(encodedTraining[letter]);
        }
        // if (letter % 10000 == 9999) {manager.status(); }
    }
                    
    // OBJ function
    double p = anderson_darling_test(frequency);
    for (int i = 0; i < SIZE; i++) {
        frequency[i] = 0.0f;
    }
    
    manager.reset();
    
    return p;
}
                