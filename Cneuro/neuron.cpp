#include "neuron.h"
#include <random>
#include <iostream>
#include "global.h"

// Constructor definition
Neuron::Neuron(int id, Manager& manager, Scheduler& scheduler) : ID(id), manager_(manager), scheduler_(scheduler) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0,1.0);
    x = 900.0f + 200.0f*dis(gen);
    y = 500.0f + 200.0f*dis(gen); 
}

void Neuron::spike(Neuron* n) {
    scheduler_.toReceive[ID] = &(n->receiver);
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

