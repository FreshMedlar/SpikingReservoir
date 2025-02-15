#include "manager.h"
#include <iostream>
#include "raylib.h"
#include <random>
#include "global.h"

Manager::Manager(int size) : size(size) { }

void Manager::createNeurons(Scheduler* sched) {
    neurons.clear();
    neurons.reserve(size);  // Reserve memory to optimize performance
    for (int i = 0; i < size; i++) {
        neurons.emplace_back(i, *this, *sched);  // Create Neuron with ID = i
    }
    std::cout << "Neurons Created" << std::endl;
}

void Manager::status() {
    std::cout << "--------Manager Status--------" << std::endl;
    std::cout << "# of neurons: " << neurons.size() << std::endl;
}


Neuron* Manager::trackConnection (Neuron& n) {
    if (neurons.empty()) {
        throw std::runtime_error("No circles available");
    }
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, neurons.size() - 1);
    int randomIndex = dis(gen);
    neurons[randomIndex].new_sender(&n); // receiving neuron gets notified
    return &neurons[randomIndex]; // sending neuron tracks the connection
}

void Manager::initialConnections() {
    connectionMatrix = std::vector<std::vector<int>>(size, std::vector<int>(size));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0,1.0);
    // create connection matrix
    for (auto& row : connectionMatrix) {
        for (auto& elem : row) {
            if (dis(gen) > 0.97) {
                elem = 1;
            } else {
                elem = 0;
            }
        }
    }
    // update neurons
    for (size_t n = 0; n < size; n++) {
        for (size_t a = 0; a < size; a++) {
            if (connectionMatrix[n][a] == 1) {
                neurons[n].connect(&neurons[a]);
                neurons[a].new_sender(&neurons[n]);
            } 
        }
    }
    std::cout << "Connections initialized" << std::endl;
}

void Manager::draw() {
    for (const Neuron& neuron : neurons) {
        DrawCircle(neuron.x, neuron.y, 10, WHITE);  
    }
    for (size_t n = 0; n < size; n++) {
        for (size_t a = 0; a < size; a++) {
            if (connectionMatrix[n][a] == 1) {
                DrawLine(neurons[n].x, neurons[n].y, neurons[a].x, neurons[a].y, WHITE);
            } 
        }
    }
}

void Manager::applyForces() {
    float repulsionStrength = 300.0f;
    float attractionStrength = 0.01f;

    // Iterate through each neuron to compute its net force
    for (auto& neuron : neurons) {
        Vector2 force = {0.0f, 0.0f};

        // --- Repulsion: All neurons push each other away ---
        for (auto& other : neurons) {
            if (&neuron != &other) {  // Avoid self-interaction
                float dx = neuron.x - other.x;
                float dy = neuron.y - other.y;
                // Compute distance (avoid division by zero with a small offset)
                float distance = std::sqrt(dx * dx + dy * dy) + 1.01f;
                // Calculate repulsive force components (inversely proportional to distance squared)
                force.x += (dx / distance) * repulsionStrength / (distance * distance);
                force.y += (dy / distance) * repulsionStrength / (distance * distance);
            }
        }
        // std::cout << force.x << std::endl;
        // --- Attraction: Pull connected neurons together ---
        for (Neuron* connected : neuron.receiver) {
            float dx = connected->x - neuron.x;
            float dy = connected->y - neuron.y;
            // Simple linear attraction force
            force.x += dx * attractionStrength;
            force.y += dy * attractionStrength;
        }

        // std::cout << force.x << std::endl;
        // --- Update neuron's position with the calculated net force ---
        neuron.x += force.x;
        neuron.y += force.y;
    }
}
