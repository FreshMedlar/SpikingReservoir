#include "manager.h"
#include <iostream>
#include "raylib.h"
#include <random>
#include "global.h"
#include <cmath>
#include <set>
#include "neuron.h"
#include <algorithm>

#define EXTRALIGHTGRAY CLITERAL(Color){ 100, 100, 100, 255 }

Manager::Manager(int size) : size(size) { }

void Manager::saveModel() {

}

void Manager::createNeurons() {
    std::uniform_real_distribution<> randum(0.0f, 1.0f);
    neurons.clear();
    neurons.reserve(size);  // Reserve memory to optimize performance
    for (short i = 0; i < size; i++) {
 
        Neuron so;
        if (randum(gen) < 0.7) { 
            constructorNeuron(so, i, 1);
        } else { 
            constructorNeuron(so, i, -1);
        }
        neurons.push_back(so);
    }
    std::cout << "Neurons Created" << std::endl;
}

void Manager::createSequentialNeurons() {
    connectionMatrix = std::vector<std::vector<float>>(size, std::vector<float>(size));
    std::uniform_real_distribution<> randum(0.0f, 1.0f);
    neurons.clear();
    neurons.reserve(size);  // Reserve memory to optimize performance
    for (short i = 0; i < size; i++) {
 
        Neuron so;
        if (randum(gen) < 0.5) { constructorNeuron(so, i, 1);
        } else { constructorNeuron(so, i, -1);}
        neurons.push_back(so);
        
        if (i>1) {connectSingle(i, i-2);}
        // std::cout << "Neuron " << i << " connected" << std::endl;
    }
    std::cout << "Neurons Created" << std::endl;
}

void Manager::createSingle(short id, bool inhibitory) {
    Neuron so;
    if (inhibitory) {constructorNeuron(so, id, -1);}
    else {constructorNeuron(so, id, 1);}
    neurons.push_back(so);
}

void Manager::connectSingle(short id, int nConns) {
    std::vector<short> connected;
    for (short i = 0; i < neurons.size(); ++i) {
        if (i != id) { // Avoid self-connections
            connected.push_back(i);
        }
    }
    while (receivers[id].size() < nConns) {
        short index = getRandomInt(0, connected.size()-1);
        int target = connected[index];

        // Avoid duplicates
        if (std::find(receivers[id].begin(), receivers[id].end(), target) == receivers[id].end()) {
            connect(id, target, getRandomFloat(0.0f, 2.0f));
        }
        connected.erase(connected.begin() + index);
    }
}

void Manager::status() {
    std::cout << "--------Manager Status--------" << std::endl;
    std::cout << "# of neurons: " << neurons.size() << std::endl;
    std::cout << connectionMatrix.size() << std::endl;
}

short Manager::randomNeuron (Neuron* nen) {
    std::uniform_int_distribution<> dis(0, neurons.size() - 1);
    int randomIndex = dis(gen);
    return randomIndex; 
}

void Manager::initialConnections(int nConns) {
    connectionMatrix = std::vector<std::vector<float>>(size, std::vector<float>(size));
    std::uniform_int_distribution<> intdis(0, size-1);
    for (Neuron& neuron : neurons) {
        std::set<int> connected;
        while (connected.size() < nConns) {
            short target = intdis(gen);
            // Avoid self-connections and duplicates
            if (target != neuron.ID && connected.find(target) == connected.end()) {
                connect(neuron.ID, target, getRandomFloat(0.0f, 2.0f));
                connected.insert(target);
            }
        }
    }
    std::cout << "Connections initialized" << std::endl;
}

void Manager::removeInputConnections(short nInput) {
    if (nInput > 0) {
        for (int neu = 0; neu < nInput; neu++) {
            inhibitory[neu] = 1;
            for (short incoming : senders[neu]) {
                disconnect(incoming, neu);
            }
        }
    } 
    if (nInput < 0) {
        for (int neu = SIZE-1; neu > (SIZE+nInput)-1; neu--) {
            inhibitory[neu] = 1;
            for (short incoming : senders[neu]) {
                disconnect(incoming, neu);
            }
        }
    }
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
    // for (size_t n = 0; n < size; n++) {
    //     for (size_t a = 0; a < size; a++) {
    //         if (connectionMatrix[n][a] > 0.0f) {
    //             DrawLine(xCoord[n], yCoord[n], xCoord[a], yCoord[a], EXTRALIGHTGRAY);
    //         } 
    //     }
    // }
    for (size_t n = 0; n < size; n++) {
        DrawCircle(xCoord[n], yCoord[n], 8, colors[n]);  
    }
}

void Manager::applyForces() {
    float repulsionStrength = 100.0f;
    float attractionStrength = 0.1f;

    // Iterate through each neuron to compute its net force
    for (size_t neuron = 0; neuron < SIZE; neuron++) {
        Vector2 force = {0.0f, 0.0f};

        // --- Repulsion: All neurons push each other away ---
        for (size_t other = 0; other < SIZE; other++) {
            if (neuron != other) {  // Avoid self-interaction
                float dx = xCoord[neuron] - xCoord[other];
                float dy = yCoord[neuron] - yCoord[other];
                // Compute distance (avoid division by zero with a small offset)
                float distance = std::sqrt(dx * dx + dy * dy) + 0.01f;
                // Calculate repulsive force components (inversely proportional to distance squared)
                // if (distance < 300.0f) {
                    force.x += (dx / distance) * repulsionStrength / (distance * distance);
                    force.y += (dy / distance) * repulsionStrength / (distance * distance);
                // }
            }
        }
        // --- Attraction: Pull connected neurons together ---
        for (short connected : receivers[neuron]) {
            float dx = xCoord[connected] - xCoord[neuron];
            float dy = yCoord[connected] - yCoord[neuron];
            float distance = std::sqrt(dx * dx + dy * dy) + 0.01f;
            
            float attractionStrength = Manager::calculateAttractionForce(distance, 10.0f);
            // Simple linear attraction force
            // if (distance > 700.0f) {
                force.x += dx * attractionStrength;
                force.y += dy * attractionStrength;
            // }
        }
        // --- Update neuron's position with the calculated net force ---
        xCoord[neuron] += force.x;
        yCoord[neuron] += force.y;
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

void Manager::drawSpikesGraph(std::vector<int>& spikeNumber) {
    int barHeight = 3;

    // DrawRectangle(10, 260, plotWidth, plotHeight, BLACK);
    for (int i = 0; i < spikeNumber.size(); i++) {
        int barWidth = spikeNumber[i];
        DrawRectangle(2 , 145 + 3*i, barWidth, barHeight, BLUE);

    }
}

void Manager::drawTotalWeight() {
    DrawRectangle(0, 80, totalSum/(SIZE/10), 35, ORANGE);
    DrawText("TOTAL WEIGHT", 10, 84, 24, WHITE);
}
 
void Manager::clustering() {
    for (short neu = 0; neu < SIZE; neu++) {
        for (short node : receivers[neu]){
            
        }
    }
    float avgCluster = 0;
    for (short i = 0; i < SIZE; i++) {
        avgCluster += receivers[i].size();
    }
    avgCluster = avgCluster/SIZE;
    DrawRectangle(0, 40, avgCluster*10, 35, RED);
    DrawText("CLUSTERING", 10, 44, 24, WHITE);
}

void Manager::drawChaos() {
    int chaos = 200;
    DrawRectangle(0, 900, chaos, 35, RED);
    DrawText("CHAOS", 10, 904, 24, WHITE);
}

void Manager::drawOrder() {
    int order = 200;
    DrawRectangle(0, 950, order, 35, BLUE);
    DrawText("ORDER", 10, 954, 24, WHITE);
}

void Manager::drawSpikeFrequencyDistribution(vector<float> freq) {
    for (int n = 0; n < freq.size(); n++) {
        DrawRectangle(1920 - n, 1080-freq[n]-10, 1, freq[n]*10+10, WHITE);
    }
}