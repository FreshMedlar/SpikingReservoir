#include "manager.h"
#include <iostream>
#include "raylib.h"
#include <random>
#include "global.h"
#include <cmath>
#include <set>

#define EXTRALIGHTGRAY CLITERAL(Color){ 100, 100, 100, 255 }

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

Neuron* Manager::randomConnection (Neuron& n) {
    if (neurons.empty()) {
        throw std::runtime_error("No neurons available");
    }
    std::uniform_int_distribution<> dis(0, neurons.size() - 1);
    int randomIndex = dis(gen);
    neurons[randomIndex].new_sender(&n); // receiving neuron gets notified
    return &neurons[randomIndex]; // sending neuron tracks the connection
}

void Manager::initialConnections() {
    connectionMatrix = std::vector<std::vector<float>>(size, std::vector<float>(size));
    std::uniform_int_distribution<> intdis(0, size-1);
    for (Neuron& neuron : neurons) {
        std::set<int> connected;
        while (connected.size() < 2) {
            int target = intdis(gen);
            // Avoid self-connections and duplicates
            if (target != neuron.ID && connected.find(target) == connected.end()) {
                neuron.connect(&neurons[target]);
                connected.insert(target);
            }
        }
    }
    std::cout << "Connections initialized" << std::endl;
}

void Manager::draw() {
    // std::cout << countNonZero(connectionMatrix) << std::endl;
    // std::cout << "Drawing with matrix at: " << &connectionMatrix << std::endl;
    for (size_t n = 0; n < size; n++) {
        for (size_t a = 0; a < size; a++) {
            if (connectionMatrix[n][a] != 0.0f) {
                DrawLine(neurons[n].x, neurons[n].y, neurons[a].x, neurons[a].y, EXTRALIGHTGRAY);
            } 
        }
    }
    for (const Neuron& neuron : neurons) {
        DrawCircle(neuron.x, neuron.y, 8, neuron.color);  
    }
}

void Manager::applyForces() {
    float repulsionStrength = 100.0f;
    float attractionStrength = 0.05f;

    // Iterate through each neuron to compute its net force
    for (auto& neuron : neurons) {
        Vector2 force = {0.0f, 0.0f};

        // --- Repulsion: All neurons push each other away ---
        for (auto& other : neurons) {
            if (&neuron != &other) {  // Avoid self-interaction
                float dx = neuron.x - other.x;
                float dy = neuron.y - other.y;
                // Compute distance (avoid division by zero with a small offset)
                float distance = std::sqrt(dx * dx + dy * dy) + 0.01f;
                // Calculate repulsive force components (inversely proportional to distance squared)
                // if (distance < 300.0f) {
                    force.x += (dx / distance) * repulsionStrength / (distance * distance);
                    force.y += (dy / distance) * repulsionStrength / (distance * distance);
                // }
            }
        }
        // std::cout << force.x << std::endl;
        // --- Attraction: Pull connected neurons together ---
        for (std::pair<Neuron*, float> connected : neuron.receiver) {
            float dx = connected.first->x - neuron.x;
            float dy = connected.first->y - neuron.y;
            float distance = std::sqrt(dx * dx + dy * dy) + 0.01f;
            
            float attractionStrength = Manager::calculateAttractionForce(distance, 10.0f);
            // Simple linear attraction force
            // if (distance > 700.0f) {
                force.x += dx * attractionStrength;
                force.y += dy * attractionStrength;
            // }
        }

        // std::cout << force.x << std::endl;
        // --- Update neuron's position with the calculated net force ---
        neuron.x += force.x;
        neuron.y += force.y;
    }
}

float Manager::calculateAttractionForce(float distance, float maxForce, float minDistance) {
    // Avoid division by zero by ensuring distance is not less than minDistance
    if (distance < minDistance) {
        distance = minDistance;
    }
    // Inverse square law: Force = maxForce / (distance^2)
    return maxForce / (distance * distance);
}

int Manager::countNonZero(std::vector<std::vector<float>>& matrix) {
    int count = 0;
    for (const auto& row : matrix) {
        for (int value : row) {
            if (value != 0.0f) {
                count++;
            }
        }
    }
    return count;
}
