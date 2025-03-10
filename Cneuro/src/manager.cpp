#include "manager.h"
#include <iostream>
#include "raylib.h"
#include <random>
#include "global.h"
#include <cmath>
#include <set>

#define EXTRALIGHTGRAY CLITERAL(Color){ 100, 100, 100, 255 }

Manager::Manager(int size) : size(size) { }

void Manager::createNeurons() {
    int mean = 10;
    int variance = 10;
    double fren;
    std::normal_distribution<float> d(mean, std::sqrt(variance));
    std::uniform_real_distribution<> randum(0.0f, 1.0f);
    neurons.clear();
    neurons.reserve(size);  // Reserve memory to optimize performance
    for (short i = 0; i < size; i++) {
        do {
            fren = d(gen);
        } while (fren < 0);  
        Neuron so;
        if (randum(gen) < 0.8) { 
            constructorNeuron(so, i, 1);
            neurons.push_back(so);
            // neurons.emplace_back(i, *this, *sched, 1); // excitatory  
        } else { 
            constructorNeuron(so, i, -1);
            neurons.push_back(so);
            // neurons.emplace_back(i, *this, *sched, -1); // inhibitory
        }
    }
    std::cout << "Neurons Created" << std::endl;
}

void Manager::status() {
    std::cout << "--------Manager Status--------" << std::endl;
    std::cout << "# of neurons: " << neurons.size() << std::endl;
}

Neuron* Manager::randomNeuron (Neuron* nen) {
    std::uniform_int_distribution<> dis(0, neurons.size() - 1);
    int randomIndex = dis(gen);
    return &neurons[randomIndex]; 
}

void Manager::initialConnections() {
    connectionMatrix = std::vector<std::vector<float>>(size, std::vector<float>(size));
    std::uniform_int_distribution<> intdis(0, size-1);
    for (Neuron& neuron : neurons) {
        std::set<int> connected;
        while (connected.size() < 2) {
            short target = intdis(gen);
            // Avoid self-connections and duplicates
            if (target != neuron.ID && connected.find(target) == connected.end()) {
                connect(neuron, target);
                connected.insert(target);
            }
        }
    }
    std::cout << "Connections initialized" << std::endl;
}

std::pair<size_t, size_t> Manager::selectWeightedRandom(const std::vector<std::vector<float>>& matrix,float totalSum) {
    // Flatten the matrix and store non-zero values and their indices
    std::vector<float> weights;
    std::vector<std::pair<size_t, size_t>> indices;
    for (size_t i = 0; i < SIZE; ++i) {
        for (size_t j = 0; j < SIZE; ++j) {
            if (matrix[i][j] != 0.0f) {
                weights.push_back(matrix[i][j]);
                indices.emplace_back(i, j);
            }
        }
    }
    // Compute cumulative distribution
    std::vector<float> cumulative;
    cumulative.reserve(weights.size());
    std::partial_sum(weights.begin(), weights.end(), std::back_inserter(cumulative));
    float actualTotal = cumulative.back();
    // Generate a random number between 0 and totalSum
    std::uniform_real_distribution<> dis(0.0f, actualTotal);
    float randomValue = dis(gen);

    // Find the corresponding index using binary search
    auto it = std::lower_bound(cumulative.begin(), cumulative.end(), randomValue);

    size_t selectedIndex = (it == cumulative.end()) ? weights.size() - 1 : std::distance(cumulative.begin(), it);
    // Return the 2D coordinates of the selected value
    return indices[selectedIndex];
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
        DrawCircle(neuron.x, neuron.y, 8, colors[neuron.ID]);  
    }
}

void Manager::applyForces() {
    float repulsionStrength = 100.0f;
    float attractionStrength = 0.1f;

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
        for (Neuron* connected : receivers[neuron.ID]) {
            float dx = connected->x - neuron.x;
            float dy = connected->y - neuron.y;
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

void Manager::countConnections(int* connections) {
    for (int sos = 0; sos < SIZE; sos++) {
        connections[sos] = senders[sos].size();
    }
}

void Manager::receiversFrequence(int* connections) {
    for (int sos = 0; sos < size; sos++) {
        connections[receivers[sos].size()]++;
    }
}

void Manager::sendersFrequence(int* connections) {
    for (int sos = 0; sos < size; sos++) {
        connections[senders[sos].size()]++;
    }
}

void Manager::drawreceiversGraph(const std::vector<int>& conn) {
    int plotWidth = 500;
    int plotHeight = 250;
    int barWidth = plotWidth / (SIZE/2);
    // int maxCount = *std::max_element(conn.begin(), conn.end());

    // DrawRectangle(10, 10, plotWidth, plotHeight, BLACK);
    for (int i = 0; i < conn.size(); i++) {
        int barHeight = conn[i]*3;
        DrawRectangle(10 + (i * barWidth), 10 + plotHeight - barHeight, barWidth, barHeight, BLUE);

    }
}

void Manager::drawSpikesGraph(const std::vector<int>& spikeNumber) {
    int plotWidth = 500;
    int plotHeight = 250;
    int barWidth = 1;
    // int maxCount = *std::max_element(conn.begin(), conn.end());

    // DrawRectangle(10, 260, plotWidth, plotHeight, BLACK);
    for (int i = 0; i < spikeNumber.size(); i++) {
        int barHeight = spikeNumber[i];
        DrawRectangle(10 + (i%1920), 10 + plotHeight*2 - barHeight, barWidth, barHeight, BLUE);

    }
}

void Manager::drawTotalWeight(const std::vector<float>& totalWeight) {
    int plotWidth = 500;
    int plotHeight = 250;
    int barWidth = 1;

    for (size_t i = 0; i < totalWeight.size(); i++) {
        if (std::isnan(totalWeight[i]) || std::isinf(totalWeight[i])) {
            continue;  // Skip invalid values
        }

        int barHeight = static_cast<int>(totalWeight[i] / 100);
        if (barHeight < 0) barHeight = 0;  // Avoid negative heights

        DrawRectangle(10 + (i % 1920), 10 + plotHeight * 3 - barHeight, barWidth, barHeight, BLUE);
    }
}

