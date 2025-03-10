#include "neuron.h"
#include <random>
#include <iostream>
#include "global.h"
#include <cmath>

// Constructor definition
Neuron::Neuron(short id, Manager& manager, Scheduler& scheduler, uint8_t inhi) : 
                ID(id), manager_(manager), scheduler_(scheduler), inhibitory(inhi) {
    std::uniform_real_distribution<> dis(0.0,1.0);
    x = 1920.0f*dis(gen);
    y = 1080.0f*dis(gen);
    actionPotential = 0;
    color = WHITE;
    timer = {ID, 60};
    timeSinceSpike = 0;
    active = true;
}

void Neuron::spike(Neuron* neuron) {
    for (std::pair<Neuron*, float> n: receivers) {forward(ID, n.first->ID, n.first->active, n.first->actionPotential, scheduler_, inhibitory);}
    for (Neuron* n : sender) { backprop(n->ID, ID, n->timeSinceSpike); }
    // if (timeSinceSpike> 1000){ scheduler_.lonelyNeurons.push_back(ID);}
    timeSinceSpike = 0;
    this->DisableObject();
}

void forward(short from, short to, bool active, short actionPotential, Scheduler scheduler_, uint8_t inhi) {
    if (active) {
        actionPotential += 30 * connectionMatrix[from][to] * inhi;
        if (actionPotential > 70) {
            actionPotential = 0;
            scheduler_.swapSpike.insert(to);
        }
    } else {
        // connectionMatrix[n][ID] -= 0.01;
    }
}

void backprop(short from, short to, int timeSinceSpike) {
    if (timeSinceSpike < 2 && connectionMatrix[from][to] < 10.0f) {
        // connectionMatrix[ID][n] += 0.01;
    }
}

float Neuron::disconnect(short n) {
    // remove the receiving neuron 
    for (int d = 0; d < receivers.size(); d++) {
        if (receivers[d].first == &neurons[n]) { 
            receivers[d].first = nullptr;
            if ( d < receivers.size() - 1 )
                receivers[d] = std::move( receivers.back() );
            receivers.pop_back();
            break;
        }
    }
    // remove this neuron from sending
    for (int jkl = 0; jkl < neurons[n].sender.size(); jkl++) {
        if (neurons[n].sender[jkl] == this) {
            if (jkl < neurons[n].sender.size() - 1) {
                neurons[n].sender[jkl] = std::move(neurons[n].sender.back());
            }
            neurons[n].sender.pop_back();
            break;
        }
    }
    //remove from connectionMatrix
    float strength = connectionMatrix[ID][n];
    connectionMatrix[ID][n] = 0.0f;
    return strength;
}

void Neuron::connect(short toConnect, float weight) {
    if (toConnect == -1) {
        toConnect = manager_.randomNeuron(this)->ID;
    }
    neurons[toConnect].sender.push_back(this);
    std::pair<Neuron*, float> p = {&neurons[toConnect], 1.0f};
    receivers.push_back(p); 
    connectionMatrix[ID][neurons[toConnect].ID] = weight;
}

void Neuron::DisableObject() {
    int targetSlot = (currentFrameIndex + COOLDOWN_FRAMES) % COOLDOWN_FRAMES;
    disableBuffer[targetSlot].push_back(this);
    active = false; // Immediately disable the object
    color = RED;
}

