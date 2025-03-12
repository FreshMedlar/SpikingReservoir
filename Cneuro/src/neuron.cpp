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

void constructorNeuron(Neuron& pre, short id, short inhi) {
    pre.ID = id;

    std::uniform_real_distribution<> dis(0.0,1.0);
    xCoord[id] =            1920.0f*dis(gen);
    yCoord[id] =            1080.0f*dis(gen);
    biases[id] =            1.0f;
    colors[id] =            WHITE;
    active[id] =            true;
    inhibitory[id] =        inhi;
    actionPotential[id] =   0;
    timeSinceSpike[id] =    0;
}

void spike(short pre) {
    for (Neuron* n: receivers[pre]) {forward(pre, n->ID);}
    for (Neuron* prepre : senders[pre]) { backprop(prepre->ID, pre); }
    // if (timeSinceSpike> 1000){ scheduler_.lonelyNeurons.push_back(ID);}
    timeSinceSpike[pre] = 0;
    DisableObject(pre);
}

void forward(short spiked, short to) {
    // 1 or -1 based on activity of postsynaptic neuron
    actionPotential[to] += (connectionMatrix[spiked][to] + biases[to]) * inhibitory[spiked];
    if (actionPotential[to] > 70) {
        actionPotential[to] = 0;
        scheduler.swapSpike.insert(to);
    }
    // connectionMatrix[spiked][to] += (LR/TEMP)*(1-(2*active[to]));
    // biases[to] += (LR/TEMP) * (1-(2*active[to]));
}

void backprop(short pre, short post) {
    if (timeSinceSpike[pre] < 2 && connectionMatrix[pre][post] < 10.0f) {
        // connectionMatrix[pre][post] += 0.01;
        // totalSum += 0.01;
    }
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

void DisableObject(short pre) {
    int targetSlot = (currentFrameIndex + COOLDOWN_FRAMES) % COOLDOWN_FRAMES;
    disableBuffer[targetSlot].push_back(pre);
    active[pre] = false; // Immediately disable the object
    colors[pre] = RED;
}

