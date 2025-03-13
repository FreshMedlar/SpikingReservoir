#include "neuron.h"
#include <random>
#include <iostream>
#include "global.h"
#include <cmath>

float biases[SIZE];
std::vector<std::vector<Neuron*>> senders(SIZE); // neurons that send to me
std::vector<std::vector<Neuron*>> receivers(SIZE); // neuron I send to
Color colors[SIZE];
int timeSinceSpike[SIZE];
bool active[SIZE];
short actionPotential[SIZE];
short inhibitory[SIZE];
float xCoord[SIZE];
float yCoord[SIZE];
short excitability[SIZE];

void constructorNeuron(Neuron& pre, short id, short inhi) {
    pre.ID = id;

    std::uniform_real_distribution<> dis(0.0,1.0);
    xCoord[id] =            1920.0f*dis(gen);
    yCoord[id] =            1080.0f*dis(gen);
    biases[id] =            1.0f;
    colors[id] =            WHITE;
    active[id] =            true;
    inhibitory[id] =        inhi;
    excitability[id] =      1;
    timeSinceSpike[id] =    1000; // to ignore the first spike
    actionPotential[id] =   0;
}

void spike(short pre) {
    for (Neuron* n: receivers[pre]) { if (active[n->ID]) {forward(pre, n->ID);}}
    for (Neuron* prepre : senders[pre]) { backprop(prepre->ID, pre); }
    // if (timeSinceSpike> 1000){ scheduler_.lonelyNeurons.push_back(ID);}
    timeSinceSpike[pre] = 0;
}

void forward(short spiked, short post) {
    // w[spiked][post] depends on timeSinceSpike[post]
    // if timeSinceSpike[post] small, w go down :(
    actionPotential[post] += (connectionMatrix[spiked][post] + biases[post]) 
                            * inhibitory[spiked] 
                            * excitability[post];
    
    if (actionPotential[post] > 70) {
        actionPotential[post] = 0;
        short targetSlot = (currentSpikeIndex + SPIKE_FRAMES) % SPIKE_BUFFER_SIZE;
        spikeBuffer[targetSlot].insert(post);
        DisableObject(post);
        excitability[post] = 1;
    } else {
        excitability[post]++;
    }
    connectionMatrix[spiked][post] -= LR/exp(timeSinceSpike[post]/TEMP);
    totalSum -= LR/exp(timeSinceSpike[post]/TEMP);
    biases[post] -= LR/exp(TEMP);
}
void backprop(short pre, short post) {
    // w[pre][post] depends on timeSinceSpike[pre] 
    // if timeSinceSpike[pre] small, w go up :)
    connectionMatrix[pre][post] += LR/exp(timeSinceSpike[pre]/TEMP);
    totalSum += LR/exp(timeSinceSpike[pre]/TEMP);
    biases[post] += LR/exp(TEMP);
}

float disconnect(short pre, short post) {
    // remove the receiving neuron 
    for (int d = 0; d < receivers[pre].size(); d++) {
        if (receivers[pre][d] == &neurons[post]) { 
            receivers[pre][d] = nullptr;
            if ( d < receivers[pre].size() - 1 )
                receivers[pre][d] = std::move( receivers[pre].back() );
            receivers[pre].pop_back();
            break;
        }
    }
    // remove this neuron from sending
    for (int jkl = 0; jkl < senders[post].size(); jkl++) {
        if (senders[post][jkl] == &neurons[pre]) {
            if (jkl < senders[post].size() - 1) {
                senders[post][jkl] = std::move(senders[post].back());
            }
            senders[post].pop_back();
            break;
        }
    }
    //remove from connectionMatrix
    float strength = connectionMatrix[pre][post];
    totalSum -= strength;
    connectionMatrix[pre][post] = 0.0f;
    return strength;
}

void connect(Neuron& pre, short toConnect, float weight) {
    if (toConnect == -1) {
        toConnect = manager.randomNeuron(&pre)->ID;
    }
    senders[toConnect].push_back(&pre);
    receivers[pre.ID].push_back(&neurons[toConnect]); 
    connectionMatrix[pre.ID][neurons[toConnect].ID] = weight;
    totalSum += weight;
}

void DisableObject(short pre, short time) {
    int targetSlot = (currentFrameIndex + time) % COOLDOWN_FRAMES;
    disableBuffer[targetSlot].push_back(pre);
    active[pre] = false; // Immediately disable the object
    colors[pre] = RED;
}

