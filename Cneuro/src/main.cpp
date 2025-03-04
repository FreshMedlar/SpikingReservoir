#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <Eigen/Dense>
#include <armadillo>
#include <ensmallen.hpp>
#include <cereal/archives/json.hpp>
// Define these to print extra informational output and warnings.
#define MLPACK_PRINT_INFO
#define MLPACK_PRINT_WARN

#include "neuron.h"
#include "manager.h"
#include "raylib.h"
#include "scheduler.h"
#include "global.h"
#include "net.h"
#include "encoder.h"

// using namespace arma;
using namespace std;

int SIZE = 500;

std::vector<int> encode(std::string s, std::map<char, int> stoi) {
    std::vector<int> encoded;
    for (auto c : s) {  encoded.push_back(stoi[c]); }
    return encoded;
}

std::string decode(std::vector<int> i, std::map<int, char> itos) {
    std::string decoded;
    for (auto num : i) {decoded += itos[num];}
    return decoded;
}

Eigen::VectorXf vectorToEigen(const std::vector<int>& vec) {
    // Create an Eigen vector of the same size
    Eigen::VectorXf eigen_vec(vec.size());
    
    // Copy data from std::vector to Eigen::VectorXf
    for (size_t i = 0; i < vec.size(); ++i) {
        eigen_vec(i) = static_cast<float>(vec[i]);  // Cast int to float
    }
    
    return eigen_vec;
}

std::pair<size_t, size_t> selectWeightedRandom(const std::vector<std::vector<float>>& matrix, 
                                                                            float totalSum) {
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

int getRandomInt(int n) {
    std::uniform_int_distribution<> randum(0, n-1);
    return randum(gen);
}

int getRandomFloat(float n) {
    std::uniform_real_distribution<> randfloat(0, n);
    return randfloat(gen);
}

int main() {
//-----------------------READ INPUT FILE----------------------
    std::string filePath = "/home/medlar/SpikingReservoir/tinyshakespeare.txt";
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
//-----------------------DATA PROCESSING-----------------------------
    size_t textLength = fileContent.length();
    std::cout << "Length of the text: " << textLength << std::endl;

    std::set<char> uniqueChars(fileContent.begin(), fileContent.end());
    std::vector<char> uniqueCharVector(uniqueChars.begin(), uniqueChars.end());
    std::vector<char> charVector(fileContent.begin(), fileContent.end());

    int splitIndex = static_cast<int>(std::round(0.9*textLength));
    std::string training = fileContent.substr(0, splitIndex);
    std::string validation = fileContent.substr(splitIndex);
    // Print the vector of characters (optional)
    std::cout << "Characters in the vector: ";
    for (char c : uniqueCharVector) {
        std::cout << c;
    }
    std::cout << std::endl;

    // Example of translating characters to numbers
    std::map<char, int> stoi;
    std::map<int, char> itos;
    for (int i = 0; i < uniqueCharVector.size(); i++) {
        stoi[uniqueCharVector[i]] = i;
        itos[i] = uniqueCharVector[i];
    }

    std::cout << "Characters translated to numbers: ";
    std::string prova = "prova";
    std::vector<int> encoded = encode(prova, stoi);
    std::string decoded = decode(encoded, itos);
    for (auto i : decoded) {
        std::cout << i << " ";
    }
    for (char c : uniqueCharVector) {
        std::cout << stoi[c] << "";
    }
    std::cout << std::endl;
//----------------------------------STUPID ENCODING------------------------------------------
    vector<int> encodedTraining = encode(training, stoi);
    vector<vector<int>> inputReservoir;
    inputReservoir.reserve(encodedTraining.size());
    for (int letter : encodedTraining) {
        vector<int> innerVec = {letter, letter+65, letter+130};
        inputReservoir.emplace_back(innerVec);
    }
//----------------------------------BY HAND MODEL------------------------------------------
    const int INPUT_SIZE = 10000;
    const int OUTPUT_SIZE = 65;
    const float LR = 0.001;
    const int NUM_SAMPLES = 1;  // Online learning

    SingleLayerNetwork network(LR, SIZE);

//----------------------------------RESERVOIR DEFINITION-----------------------------------
    Manager manager(SIZE);
    Scheduler scheduler(SIZE);
    manager.createNeurons(&scheduler);
    manager.initialConnections();
    manager.status();
    
    int SPIKE_SAMPLING = 10;
    int frameCounter = 0;
    std::vector<int> connectionsPerNeuron(SIZE, 0);
    std::uniform_real_distribution<> dis(0.0,1.0);
    std::uniform_int_distribution<> disreal(0, SIZE-1);
//----------------------------------VISUALS-----------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Raylib - Circle Manager");
    ToggleFullscreen();    
    SetTargetFPS(0);
//----------------------------------DATA--------------------------------------------------
    // Eigen::MatrixXf spikeHistory = Eigen::MatrixXf::Zero(10, 1000); 
    vector<int> spikeHistory;
    spikeHistory.reserve(SPIKE_SAMPLING);
    vector<int> spikeNumber;
    spikeNumber.reserve(500);
    spikeNumber.push_back(0);
    spikeNumber.resize(500);
    vector<float> totalWeight;
    totalWeight.reserve(500);
    totalWeight.resize(500);
//---------------------------------KEY---------------------------------------------------
    float growthProb = 0.66f;
    int toRemove;
    int fromRemove;
    float toDistribute = 0.0f;
    int from;
    int to;
//----------------------------------TRAINING LOOP-----------------------------------------
    int epoch = 0;
    bool train = false;
    bool draw = true;
    bool graph = true;
    while (!WindowShouldClose()) {
    // for (int s = 0; s < 1; s++) {
        float epoch_loss = 0;

        BeginDrawing();
        ClearBackground(BLACK);

        // NEURONS DRAWING 
        if (draw) {
            // scheduler.changeColor();
            manager.draw();
            if (frameCounter%1 ==0) {manager.applyForces();}
        }
        if (graph) {
            // GRAPHS DRAWING
            if (frameCounter%1 == 0) {
                connectionsPerNeuron.clear();
                connectionsPerNeuron.resize(SIZE, 0); 
                // EITHER, NOT BOTH
                manager.receiverFrequence(connectionsPerNeuron.data()); 
                // manager.senderFrequence(connectionsPerNeuron.data());
            }
            manager.drawReceiverGraph(connectionsPerNeuron); // Draw the plot
            manager.drawSpikesGraph(spikeNumber);
            // manager.drawTotalWeight(totalWeight);
        }
        // FPS
        int fps = GetFPS();
        DrawText(TextFormat("FPS: %d", fps), 10, 10, 20, GREEN); 

        // RANDOM PRUNING

        float totalSum = 0.0f;
        for (const auto& wor : connectionMatrix) {
            totalSum = std::accumulate(wor.begin(), wor.end(), totalSum);
        }
        totalWeight[(epoch)%500] = totalSum;
//----------------------------RESERVOIR UPDATE-----------------------------------
        for(int sample = 0; sample < SPIKE_SAMPLING; sample++) {
            frameCounter++;
//----------------------------REFRACTORY PERIOD------------------------------
            for (Neuron* obj : disableBuffer[currentFrameIndex]) {
                obj->active = true;
                obj->color = WHITE;
            }
            disableBuffer[currentFrameIndex].clear(); // Reset the slot
            // Advance the ring buffer index
            currentFrameIndex = (currentFrameIndex + 1) % COOLDOWN_FRAMES;


//------------------------------RANDOM RESTRUCTURING---------------------------
            toRemove = disreal(gen);
            if (!neurons[toRemove].receiver.empty()) {
                // get the ID of the neuron we disconnect from
                fromRemove = getRandomInt(neurons[toRemove].receiver.size()); // Get connection index 
                Neuron* targetNeuron = neurons[toRemove].receiver[fromRemove].first;
                fromRemove = targetNeuron->ID;
                // disconnect and get connection strength
                toDistribute = neurons[toRemove].disconnect(fromRemove);

                // if a connection has been removed, distribute its strength
                int sjdfo = static_cast<int>(toDistribute); 
                for (int a = 0; a < sjdfo; a++) {
                    if (dis(gen) < growthProb) {
                        float weight = static_cast<float>(SIZE);
                        auto [row, col] = selectWeightedRandom(connectionMatrix, totalSum);
                        connectionMatrix[row][col] += 1.0f;
                        // connectionMatrix[toRemove][] += 1.0f;
                    } else {
                        from = disreal(gen);
                        to = disreal(gen);
                        if (connectionMatrix[from][to] != 0.0f){
                            connectionMatrix[from][to] += 1.0f;
                        } else {
                            neurons[from].connect(to, 1.0f);
                        }
                    }
                }
            }
            toDistribute = 0.0f;

//------------------------INPUT----------------------------
            for(int ins : inputReservoir[epoch]) {
                scheduler.toSpike.insert(ins);
            }
            //X(t) FOR THE MODEL
            if (train) {
                for (int ins : scheduler.toSpike){
                    spikeHistory.push_back(ins+(SIZE*sample));
                }
            }
            //RESERVOIR
            spikeNumber[((epoch*10)+sample)%500] = scheduler.toSpike.size();
            scheduler.update();
            // scheduler.synaptoGenesis();
        }
//-----------------------------MODEL BY HAND-------------------------------
        if (train) {
            Eigen::VectorXf output = network.forward_sparse(spikeHistory);
            output = network.softmax(output);
            epoch_loss += network.compute_loss(output, encodedTraining[epoch+1]);
            Eigen::VectorXf d_input = network.backward(spikeHistory, output, encodedTraining[epoch]); // 10000, 
            spikeHistory.clear();
        }
        if (train) {
        std::cout << "Epoch " << epoch 
                 << " | Avg Loss: " << epoch_loss/1
                 << std::endl;
        }
        EndDrawing();
        epoch++;
        
    }
    CloseWindow();
    return 0;
}


