#include "manager.h"
#include <vector>
#include "neuron.h"
#include <iostream>
#include "raylib.h"

Manager::Manager(int size) : size(size) {
    Manager::createNeurons();
}

void Manager::createNeurons() {
    neurons.clear();
    neurons.reserve(size);  // Reserve memory to optimize performance
    for (int i = 0; i < size; i++) {
        neurons.emplace_back(i);  // Create Neuron with ID = i
    }
}

void Manager::status() {
    std::cout << "---Manager Status---" << std::endl;
    std::cout << "# of neurons: " << neurons.size() << std::endl;
}

void Manager::draw() {
    for (const Neuron& neuron : neurons) {
        DrawCircle(400, 300, 25, WHITE);  
    }
}