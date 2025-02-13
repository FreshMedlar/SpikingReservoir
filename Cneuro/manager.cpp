#include "manager.h"
#include <iostream>
#include "raylib.h"
#include <random>

Manager::Manager(int newSize) : size(newSize) {
    Manager::createNeurons();
}

void Manager::createNeurons() {
    neurons.clear();
    neurons.reserve(size);  // Reserve memory to optimize performance
    for (int i = 0; i < size; i++) {
        neurons.emplace_back(i, *this);  // Create Neuron with ID = i
    }
}

void Manager::status() {
    std::cout << "--------Manager Status--------" << std::endl;
    std::cout << "# of neurons: " << neurons.size() << std::endl;
}

void Manager::draw() {
    for (const Neuron& neuron : neurons) {
        DrawCircle(800, 600, 5, WHITE);  
    }
}

Neuron& Manager::connect (Neuron& n) {
    if (neurons.empty()) {
        throw std::runtime_error("No circles available");
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, neurons.size() - 1);
    int randomIndex = dis(gen);
    neurons[randomIndex].new_sender(n); // receiving neuron gets notified
    return neurons[randomIndex]; // sending neuron tracks the connection
}