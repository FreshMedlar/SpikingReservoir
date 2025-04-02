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

Manager::Manager(int size, Simulation& simulation) 
    :   size(size), 
        simulation(simulation), 
        neurons(simulation.neurons),
        connectionMatrix(simulation.connectionMatrix)
{

}

void Manager::saveModel() {

}

void Manager::createNeurons() {
    cout << "Creating neurons" << endl;
    std::uniform_real_distribution<> randum(0.0f, 1.0f);
    neurons.clear();
    neurons.reserve(size);  // Reserve memory to optimize performance
    for (short i = 0; i < size; i++) {
        // Neuron so(simulation);
        if (randum(gen) < 0.7) { 
            // constructorNeuron(so, i, 1);
            Neuron so(simulation, i, 1);
            neurons.push_back(so);
        } else { 
            // constructorNeuron(so, i, -1);
            Neuron so(simulation, i, -1);
            neurons.push_back(so);
        }
        
    }
    std::cout << "Neurons Created" << std::endl;
}

void Manager::createSequentialNeurons() {
    std::uniform_real_distribution<> randum(0.0f, 1.0f);
    neurons.clear();
    neurons.reserve(size);  // Reserve memory to optimize performance
    for (short i = 0; i < size; i++) {
 
        // Neuron so;
        if (randum(gen) < 0.5) { 
            Neuron so(simulation, i, 1); //constructorNeuron(so, i, 1);
            neurons.push_back(so);
        } else { 
            Neuron so(simulation, i, -1); //constructorNeuron(so, i, -1);}
            neurons.push_back(so);
        } 
        
        
        if (i>1) {connectSingle(i, i-2);}
        // std::cout << "Neuron " << i << " connected" << std::endl;
    }
    std::cout << "Neurons Created" << std::endl;
}

void Manager::createSingle(short id, bool inhibitory) {
    if (inhibitory) {
        // constructorNeuron(so, id, -1);
        Neuron so(simulation, id, -1);
        neurons.push_back(so);
    } else {
        // constructorNeuron(so, id, 1);
        Neuron so(simulation, id, -1);
        neurons.push_back(so);
    }
    
}

void Manager::connectSingle(short id, int nConns) {
    std::vector<short> connected;
    for (short i = 0; i < neurons.size(); ++i) {
        if (i != id) { // Avoid self-connections
            connected.push_back(i);
        }
    }
    while (simulation.receivers[id].size() < nConns) {
        short index = getRandomInt(0, connected.size()-1);
        int target = connected[index];

        // Avoid duplicates
        if (std::find(simulation.receivers[id].begin(), simulation.receivers[id].end(), target) == simulation.receivers[id].end()) {
            neurons[id].connect(id, target, getRandomFloat(0.0f, 2.0f));
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
    cout << "Initializing connections" << endl;
    std::uniform_int_distribution<> intdis(0, size-1);
    for (Neuron& neuron : neurons) {
        std::set<int> connected;
        while (connected.size() < nConns) {
            short target = intdis(gen);
            // Avoid self-connections and duplicates
            if (target != neuron.ID && connected.find(target) == connected.end()) {
                neurons[target].connect(neuron.ID, target, getRandomFloat(0.0f, 2.0f));
                connected.insert(target);
            }
        }
    }
    std::cout << "Connections initialized" << std::endl;
}

void Manager::removeInputConnections(short nInput) {
    cout << "Removing input connections" << endl;
    if (nInput > 0) {
        for (int neu = 0; neu < nInput; neu++) {
            simulation.inhibitory[neu] = 1;
            for (short incoming : simulation.senders[neu]) {
                neurons[incoming].disconnect(incoming, neu);
            }
        }
    } 
    if (nInput < 0) {
        for (int neu = size-1; neu > (size+nInput)-1; neu--) {
            simulation.inhibitory[neu] = 1;
            for (short incoming : simulation.senders[neu]) {
                neurons[incoming].disconnect(incoming, neu);
            }
        }
    }
    std::cout << "Input connections removed" << std::endl;
}

// std::pair<size_t, size_t> Manager::selectWeightedRandom(const std::vector<std::vector<float>>& matrix,float totalSum) {
//     // Flatten the matrix and store non-zero values and their indices
//     std::vector<float> weights;
//     std::vector<std::pair<size_t, size_t>> indices;
//     for (size_t i = 0; i < size; ++i) {
//         for (size_t j = 0; j < size; ++j) {
//             if (matrix[i][j] != 0.0f) {
//                 weights.push_back(matrix[i][j]);
//                 indices.emplace_back(i, j);
//             }
//         }
//     }
//     // Compute cumulative distribution
//     std::vector<float> cumulative;
//     cumulative.reserve(weights.size());
//     std::partial_sum(weights.begin(), weights.end(), std::back_inserter(cumulative));
//     float actualTotal = cumulative.back();
//     // Generate a random number between 0 and totalSum
//     std::uniform_real_distribution<> dis(0.0f, actualTotal);
//     float randomValue = dis(gen);

//     // Find the corresponding index using binary search
//     auto it = std::lower_bound(cumulative.begin(), cumulative.end(), randomValue);

//     size_t selectedIndex = (it == cumulative.end()) ? weights.size() - 1 : std::distance(cumulative.begin(), it);
//     // Return the 2D coordinates of the selected value
//     return indices[selectedIndex];
// }

void Manager::reset() {
    for (auto& spikeVector : simulation.spikeBuffer) {
        spikeVector.clear();
    }
    simulation.currentSpikeIndex = 0; // Reset the index as well
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
        DrawCircle(simulation.xCoord[n], simulation.yCoord[n], 8, simulation.colors[n]);  
    }
}

void Manager::applyForces() {
    float repulsionStrength = 100.0f;
    float attractionStrength = 0.1f;

    // Iterate through each neuron to compute its net force
    for (size_t neuron = 0; neuron < size; neuron++) {
        Vector2 force = {0.0f, 0.0f};

        // --- Repulsion: All neurons push each other away ---
        for (size_t other = 0; other < size; other++) {
            if (neuron != other) {  // Avoid self-interaction
                float dx = simulation.xCoord[neuron] - simulation.xCoord[other];
                float dy = simulation.yCoord[neuron] - simulation.yCoord[other];
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
        for (short connected : simulation.receivers[neuron]) {
            float dx = simulation.xCoord[connected] - simulation.xCoord[neuron];
            float dy = simulation.yCoord[connected] - simulation.yCoord[neuron];
            float distance = std::sqrt(dx * dx + dy * dy) + 0.01f;
            
            float attractionStrength = Manager::calculateAttractionForce(distance, 10.0f);
            // Simple linear attraction force
            // if (distance > 700.0f) {
                force.x += dx * attractionStrength;
                force.y += dy * attractionStrength;
            // }
        }
        // --- Update neuron's position with the calculated net force ---
        simulation.xCoord[neuron] += force.x;
        simulation.yCoord[neuron] += force.y;
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
    for (int sos = 0; sos < size; sos++) {
        connections[sos] = simulation.senders[sos].size();
    }
}

// number of outgoing connections
void Manager::receiversFrequence(int* connections) {
    for (int sos = 0; sos < size; sos++) {
        connections[simulation.receivers[sos].size()]++;
    }
}

void Manager::sendersFrequence(int* connections) {
    for (int sos = 0; sos < size; sos++) {
        connections[simulation.senders[sos].size()]++;
    }
}

void Manager::drawreceiversGraph(const std::vector<int>& conn) {
    int plotWidth = 500;
    int plotHeight = 250;
    int barWidth = plotWidth / (size/2);
    // int maxCount = *std::max_element(conn.begin(), conn.end());

    // DrawRectangle(10, 10, plotWidth, plotHeight, BLACK);
    for (int i = 0; i < conn.size(); i++) {
        int barHeight = conn[i]*3;
        DrawRectangle(10 + (i * barWidth), 10 + plotHeight - barHeight, barWidth, barHeight, BLUE);

    }
}

void Manager::drawSpikesGraph(std::vector<int>& spikeNumber) {
    int barHeight = 2;

    // DrawRectangle(10, 260, plotWidth, plotHeight, BLACK);
    for (int i = 0; i < spikeNumber.size(); i++) {
        int barWidth = spikeNumber[i];
        DrawRectangle(2 , 145 + 2*i, barWidth, barHeight, BLUE);

    }
}

// void Manager::drawTotalWeight() {
//     DrawRectangle(0, 80, totalSum/(size/10), 35, ORANGE);
//     DrawText("TOTAL WEIGHT", 10, 84, 24, WHITE);
// }
 
void Manager::clustering() {
    for (short neu = 0; neu < size; neu++) {
        for (short node : simulation.receivers[neu]){
            
        }
    }
    float avgCluster = 0;
    for (short i = 0; i < size; i++) {
        avgCluster += simulation.receivers[i].size();
    }
    avgCluster = avgCluster/size;
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
        DrawRectangle(1910 - n, 1070-freq[n], 1, freq[n], WHITE);
    }
}



// RESTRUCTURING
// for (int restruct = 0; restruct < 10000; restruct++) {
//     toRemove = disreal(gen);
//     if (!simulation.receivers[toRemove].empty()) {
//         // get the ID of the neuron we disconnect from
//         fromRemove = getRandomInt(simulation.receivers[toRemove].size()); // Get connection index 
//         Neuron* targetNeuron = simulation.receivers[toRemove][fromRemove];
//         fromRemove = targetNeuron->ID;
//         // disconnect and get connection strength
//         toDistribute = disconnect(fromRemove, toRemove);
//         // if a connection has been removed, distribute its strength
//         int sjdfo = static_cast<int>(toDistribute); 
//         for (int a = 0; a < sjdfo; a++) {
//             if (dis(gen) < growthProb) {
//                 float weight = static_cast<float>(size);
//                 auto [row, col] = manager.selectWeightedRandom(connectionMatrix, totalSum);
//                 connectionMatrix[row][col] += 1.0f;
//                 // connectionMatrix[toRemove][] += 1.0f;
//             } else {
//                 from = disreal(gen);
//                 to = disreal(gen);
//                 if (connectionMatrix[from][to] != 0.0f){
//                     connectionMatrix[from][to] += 1.0f;
//                 } else {
//                     connect(neurons[from], to, 1.0f);
//                 }
//             }
//         }
//     }
//     toDistribute = 0.0f;
// }
// restructure = false; 