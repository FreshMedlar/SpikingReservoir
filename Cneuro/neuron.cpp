#include "neuron.h"

// Constructor definition
Neuron::Neuron(int id, Manager& manager) : ID(id), manager_(manager) {}

void Neuron::connect() {
    receiver.push_back(manager_.connect(*this)); 
}

void Neuron::new_sender(Neuron& n) {
    sender.push_back(n);
}

