#include "scheduler.h"

Scheduler::Scheduler(int size, Simulation& simulation) 
        :   size(size), 
            simulation(simulation), 
            neurons(simulation.neurons),
            connectionMatrix(simulation.connectionMatrix)   
{
}

void Scheduler::updateColor() {
    for (short obj : simulation.colorBuffer[simulation.currentColorIndex]) {
        simulation.colors[obj] = WHITE;
    }
    simulation.colorBuffer[simulation.currentColorIndex].clear(); // Reset the slot
    // Advance the ring buffer index
    simulation.currentColorIndex = (simulation.currentColorIndex + 1) % simulation.COLOR_FRAMES;
}

void Scheduler::pruningAndDecay() {
    // #pragma omp parallel for
    for (size_t row = 0; row < size; row++) {
        if (simulation.biases[row] < 0.1) {
            for (size_t col = 0; col < size; col++) {
                if (connectionMatrix[col][row] < 0.1f){
                    neurons[col].disconnect(col, row);
                }
                connectionMatrix[row][col] -= 0.001f;
            }
        } 
        simulation.biases[row] -= 0.001f;
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
            
// Neuron& Scheduler::pickNeuron() {
//     // Calculate the total friendliness
//     int totalreceivers = std::accumulate(neurons.begin(), neurons.end(), 0,
//     [](int sum, const Neuron& neuron) {
//         return sum + simulation.receivers[neuron.ID].size();
//     });
    
//     // Generate a random number between 0 and totalFriendliness
//     std::uniform_int_distribution<> dis(0, totalreceivers - 1);
//     int randomNumber = dis(gen);
    
//     // Select a neuron based on the random number
//     for (auto& neuron : neurons) {
//         if (randomNumber < receivers[neuron.ID].size()+2) {
//             return neuron;
//         }
//         randomNumber -= receivers[neuron.ID].size();
//     }
    
//     // In case of rounding errors or empty vector, return the last neuron
//     return neurons.back();
// }
            
            
// void Scheduler::update() {}

void Scheduler::step (short letter) {
    // INPUT
    short input = size - letter;
    // for (short )
        if (simulation.active[input] == true) {
            simulation.spikeBuffer[simulation.currentSpikeIndex].push_back(input);
            simulation.active[input] = false;
        }
    // GLOBAL UPDATES
    for (int neu = 0; neu < size; neu++) {
        simulation.timeSinceSpike[neu] += 1;
        // excitability[neu] -= 0.06f * (1.0f - excitability[neu]);
    }
    // SPIKES 
    for (short neur : simulation.spikeBuffer[simulation.currentSpikeIndex]) { neurons[neur].spike(neur); } 
                                                        //active[neur] = true;}
    simulation.spikeBuffer[simulation.currentSpikeIndex].clear(); // Reset the slot
    simulation.currentSpikeIndex = (simulation.currentSpikeIndex + 1) % simulation.SPIKE_BUFFER_SIZE;
}

