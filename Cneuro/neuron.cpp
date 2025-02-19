#include "neuron.h"
#include <random>
#include <iostream>
#include "global.h"

// Constructor definition
Neuron::Neuron(int id, Manager& manager, Scheduler& scheduler) : 
                ID(id), manager_(manager), scheduler_(scheduler) {
    std::uniform_real_distribution<> dis(0.0,1.0);
    x = 1920.0f*dis(gen);//460.0f + 1000.0f*dis(gen);
    y = 1080.0f*dis(gen);//250.0f + 580.0f*dis(gen); 
    actionPotential = 0;
    color = WHITE;
    timer = {ID, 60};
    timeSinceSpike = 0;
}

void Neuron::spike(Neuron* neuron) {
    // scheduler_.toAdd.push_back(ID);
    for (std::pair<Neuron*, float> n: receiver) {n.first->forward(ID);}
    for (Neuron* n : sender) {n->backprop(ID);}
    timeSinceSpike = 0;
    color = RED;
    scheduler_.trackColor.push_back(timer);
}

void Neuron::forward(int n) {
    actionPotential += 30 * connectionMatrix[n][ID];
    if (actionPotential > 100) {
        actionPotential = 0;
        scheduler_.toSpike.push_back(ID);
    }
}

void Neuron::backprop(int n) {
    if (connectionMatrix[ID][n] > 10) {
        if (timeSinceSpike > 30) {
            connectionMatrix[ID][n] -= 0.01;
        } else {
            connectionMatrix[ID][n] += (-(timeSinceSpike-30)/100);
        }
    } else {
        // TODO need be removed
    }
}

void Neuron::connect(Neuron* n) {
    if (n == nullptr) {
        std::pair<Neuron*, float> p = {manager_.randomConnection(*this), 1};
        receiver.push_back(p); 
    } else {
        std::pair<Neuron*, float> p = {n, 1};
        receiver.push_back(p);
    }
}

void Neuron::new_sender(Neuron* n) {
    sender.push_back(n);
}
