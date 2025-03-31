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
            
            
// void Scheduler::update() {}

void Scheduler::step (short letter) {
    // INPUT
    short input = SIZE - letter;
    if (active[input] == true) {
        spikeBuffer[currentSpikeIndex].push_back(input);
        active[input] = false;
    }
    // GLOBAL UPDATES
    for (int neu = 0; neu < SIZE; neu++) {
        timeSinceSpike[neu] += 1;
        // excitability[neu] -= 0.06f * (1.0f - excitability[neu]);
    }
    // SPIKES 
    for (short neur : spikeBuffer[currentSpikeIndex]) { spike(neur); } 
                                                        //active[neur] = true;}
    spikeBuffer[currentSpikeIndex].clear(); // Reset the slot
    currentSpikeIndex = (currentSpikeIndex + 1) % SPIKE_BUFFER_SIZE;
}
                
double Scheduler::simulation() { //, vector<int>& connectionsPerNeuron) {
    SingleLayerNetwork network(LR, SIZE);
    bool draw = false;
    float loss;
    double epoch_loss = 0.0f;
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

            if (cycle == 9){
                // cout << spikeBuffer[currentSpikeIndex].size() << endl;
                short target = encodedTraining[letter+1];
                Eigen::VectorXf output = network.forward_sparse(spikeBuffer[currentSpikeIndex]);
                output = network.softmax(output);
                loss = network.compute_loss(output, target);
                epoch_loss += loss;
                Eigen::VectorXf d_input = network.backward(spikeBuffer[currentSpikeIndex], output, target); // 10000, 
            }
        }
    }
                    
    // OBJ function
    // epoch_loss = anderson_darling_test(frequency);
    // for (int i = 0; i < SIZE; i++) {
    //     frequency[i] = 0.0f;
    // }
    
    manager.reset();
    
    return epoch_loss;
}
                