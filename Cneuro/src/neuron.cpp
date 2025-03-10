#include "neuron.h"
#include <random>
#include <iostream>
#include "global.h"
#include <cmath>

std::vector<std::vector<Neuron*>> senders(SIZE); // neurons that send to me
std::vector<std::vector<Neuron*>> receivers(SIZE); // neuron I send to
Color colors[SIZE];
int timeSinceSpike[SIZE];

void constructorNeuron(Neuron& pre, short id, uint8_t inhi) {
    pre.ID = id;
    pre.inhibitory = inhi;
    std::uniform_real_distribution<> dis(0.0,1.0);
    pre.x = 1920.0f*dis(gen);
    pre.y = 1080.0f*dis(gen);
    pre.actionPotential = 0;
    colors[id] = WHITE;
    timeSinceSpike[id] = 0;
    pre.active = true;
}

void spike(int neur) {
    for (Neuron* n: receivers[neur]) {forward(neur, n->ID, n->active, n->actionPotential, neurons[neur].inhibitory);}
    for (Neuron* n : senders[neur]) { backprop(n->ID, neur, timeSinceSpike[n->ID]); }
    // if (timeSinceSpike> 1000){ scheduler_.lonelyNeurons.push_back(ID);}
    timeSinceSpike[neur] = 0;
    DisableObject(neurons[neur]);
}

void forward(short from, short to, bool active, short actionPotential, uint8_t inhi) {
    if (active) {
        actionPotential += 70 * connectionMatrix[from][to] * inhi;
        if (actionPotential > 70) {
            actionPotential = 0;
            scheduler.swapSpike.insert(to);
        }
    } else {
        // connectionMatrix[n][ID] -= 0.01;
        // totalSum -= 0.01;
    }
}

void backprop(short from, short to, int timeSinceSpike) {
    if (timeSinceSpike < 2 && connectionMatrix[from][to] < 10.0f) {
        // connectionMatrix[ID][n] += 0.01;
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

void DisableObject(Neuron& pre) {
    int targetSlot = (currentFrameIndex + COOLDOWN_FRAMES) % COOLDOWN_FRAMES;
    disableBuffer[targetSlot].push_back(&pre);
    pre.active = false; // Immediately disable the object
    colors[pre.ID] = RED;
}

