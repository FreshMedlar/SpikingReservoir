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
}

void Neuron::spike(Neuron* neuron) {
    // scheduler_.toAdd.push_back(ID);
    for (Neuron* n : receiver) {n->impulse(this);}
    color = RED;
}

void Neuron::impulse(Neuron* neuron) {
    actionPotential += 30;
    if (actionPotential > 100) {scheduler_.toSpike.push_back(ID);}
}

void Neuron::connect(Neuron* n) {
    if (n == nullptr) {
        receiver.push_back(manager_.trackConnection(*this)); 
    } else {
        receiver.push_back(n);
    }
}

void Neuron::new_sender(Neuron* n) {
    sender.push_back(n);
}

