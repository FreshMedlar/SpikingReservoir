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
// #define MLPACK_PRINT_INFO
// #define MLPACK_PRINT_WARN

#include "neuron.h"
#include "manager.h"
#include "raylib.h"
#include "scheduler.h"
#include "global.h"
#include "net.h"
#include "encoder.h"

// using namespace arma;
using namespace std;

int SIZE = 1000;

// The text to encode in input, in output a vector with the letters encoded in integers
std::vector<short> encode(std::string s, std::map<char, short> stoi) {
    std::vector<short> encoded;
    for (auto letters : s) {  encoded.push_back(stoi[letters]); }
    return encoded;
}

std::string decode(std::vector<short> i, std::map<short, char> itos) {
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

    std::set<char> uniqueCharsSet(fileContent.begin(), fileContent.end());
    std::vector<char> uniqueChars(uniqueCharsSet.begin(), uniqueCharsSet.end());
    std::vector<char> charVector(fileContent.begin(), fileContent.end());

    int splitIndex = static_cast<int>(std::round(0.9*textLength));
    std::string training = fileContent.substr(0, splitIndex);
    std::string validation = fileContent.substr(splitIndex);
    // Print the vector of characters (optional)
    std::cout << "Characters in the vector: ";
    for (char c : uniqueChars) {
        std::cout << c;
    }
    std::cout << std::endl;

    // Example of translating characters to numbers
    std::map<char, short> stoi;
    std::map<short, char> itos;
    for (short i = 0; i < uniqueChars.size(); i++) {
        stoi[uniqueChars[i]] = i;
        itos[i] = uniqueChars[i];
    }

    std::cout << "Characters translated to numbers: ";
    std::string prova = "prova";
    std::vector<short> encoded = encode(prova, stoi);
    std::string decoded = decode(encoded, itos);
    for (auto i : decoded) {
        std::cout << i << " ";
    }
    for (char c : uniqueChars) {
        std::cout << stoi[c] << "";
    }
    std::cout << std::endl;
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
//----------------------------------STUPID ENCODING------------------------------------------
    vector<short> encodedTraining = encode(training, stoi); // whole training
    vector<vector<vector<short>>> inputReservoir;
    // inputReservoir.reserve(encodedTraining.size());
    // int cycle = 0;
    // for (short letter : encodedTraining) {
    //     vector<short> innerVec = {letter, static_cast<short>(letter + 65), static_cast<short>(letter + 130)};
    //     for (int as = 0; as < SPIKE_SAMPLING;as++) {
    //         inputReservoir[cycle].push_back(innerVec);
    //     }
    //     cycle += 1;
    // }
//---------------------------------SECOND ENCODING------------------------------------------
    
    // the idea is to have a normal distribution around the encoded letter/symbol
    vector<short> encodedUniqueChars;
    for (char letter : uniqueChars){
        string s = {letter};
        encodedUniqueChars.push_back(encode(s, stoi)[0]);
    }
    vector<vector<vector<short>>> spikeEncodedChars (encodedUniqueChars.size());
    for (int s = 0; s < encodedUniqueChars.size(); s++) {
        for (short o = 0; o < SPIKE_SAMPLING; o++){
            vector<short> innerVec;
            for (int ra = 0; ra < 2; ra++){ 
                innerVec.push_back(static_cast<short>(getRandomInt(65)));}
            spikeEncodedChars[s].push_back(innerVec);
            innerVec.clear();
        }
    }
    for (short letter : encodedTraining) {
        inputReservoir.push_back(spikeEncodedChars[letter]);
    }
//----------------------------------BY HAND MODEL------------------------------------------
    const int INPUT_SIZE = 5000;
    const int OUTPUT_SIZE = 65;
    const float LR = 0.0001;
    const int NUM_SAMPLES = 1;  // Online learning

    SingleLayerNetwork network(LR, SIZE);
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
    spikeNumber.reserve(SIZE);
    spikeNumber.push_back(0);
    spikeNumber.resize(SIZE);
    vector<float> totalWeight;
    totalWeight.reserve(SIZE);
    totalWeight.resize(SIZE);
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
    bool restructure = false;
    while (!WindowShouldClose()) {
        float epoch_loss = 0;
        for (int pass = 0; pass < 100; pass++) {

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
                    manager.receiversFrequence(connectionsPerNeuron.data()); 
                    // manager.senderFrequence(connectionsPerNeuron.data());
                }
                manager.drawreceiversGraph(connectionsPerNeuron); // Draw the plot
                manager.drawSpikesGraph(spikeNumber);
                manager.drawTotalWeight(totalWeight);
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
// std::cout << totalSum << std::endl;

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
                if (restructure) {
                    for (int restruct = 0; restruct < 10000; restruct++) {
                        toRemove = disreal(gen);
                        if (!neurons[toRemove].receivers.empty()) {
                            // get the ID of the neuron we disconnect from
                            fromRemove = getRandomInt(neurons[toRemove].receivers.size()); // Get connection index 
                            Neuron* targetNeuron = neurons[toRemove].receivers[fromRemove].first;
                            fromRemove = targetNeuron->ID;
                            // disconnect and get connection strength
                            toDistribute = neurons[toRemove].disconnect(fromRemove);

                            // if a connection has been removed, distribute its strength
                            int sjdfo = static_cast<int>(toDistribute); 
                            for (int a = 0; a < sjdfo; a++) {
                                if (dis(gen) < growthProb) {
                                    float weight = static_cast<float>(SIZE);
                                    auto [row, col] = manager.selectWeightedRandom(connectionMatrix, totalSum);
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
                    }
                    restructure = false; 
                }
    //-------------------------------INPUT----------------------------------------------
                // we queue N neurons to spike next
                for (short ll : inputReservoir[epoch][sample]){
                    scheduler.toSpike.insert(ll);
                }
                // OR
                // for(int ins : inputReservoir[epoch][sample]) {
                //     scheduler.toSpike.insert(ins);
                // }
                //X(t) FOR THE MODEL
                if (train) {
                    for (auto ins : scheduler.toSpike){
                        spikeHistory.push_back(ins+(SIZE*sample));
                    }
                }
                //RESERVOIR
                spikeNumber[((epoch*10)+sample)%500] = scheduler.toSpike.size();
                scheduler.update();
                // scheduler.synaptoGenesis();
                
                // DECAY
                for (Neuron neur : neurons) {
                    neur.actionPotential -= 5;
                }
            }
    //-----------------------------MODEL BY HAND-------------------------------
            if (train) {
                Eigen::VectorXf output = network.forward_sparse(spikeHistory);
                output = network.softmax(output);
                epoch_loss += network.compute_loss(output, encodedTraining[epoch+1]);
                Eigen::VectorXf d_input = network.backward(spikeHistory, output, encodedTraining[epoch]); // 10000, 
                spikeHistory.clear();
            }
            EndDrawing();
            epoch++;
        }    
        if (train) {
            std::cout << "Epoch " << epoch/100 
                    << " | Avg Loss: " << epoch_loss/100
                    << std::endl;
        }
    }
    CloseWindow();
    return 0;
}


//     while (!WindowShouldClose()) {
//         float epoch_loss = 0;
//         for (int pass = 0; pass < 100; pass++) {

//             BeginDrawing();
//             ClearBackground(BLACK);

//             // NEURONS DRAWING 
//             if (draw) {
//                 // scheduler.changeColor();
//                 manager.draw();
//                 if (frameCounter%1 ==0) {manager.applyForces();}
//             }
//             if (graph) {
//                 // GRAPHS DRAWING
//                 if (frameCounter%1 == 0) {
//                     connectionsPerNeuron.clear();
//                     connectionsPerNeuron.resize(SIZE, 0); 
//                     // EITHER, NOT BOTH
//                     manager.receiversFrequence(connectionsPerNeuron.data()); 
//                     // manager.senderFrequence(connectionsPerNeuron.data());
//                 }
//                 manager.drawreceiversGraph(connectionsPerNeuron); // Draw the plot
//                 manager.drawSpikesGraph(spikeNumber);
//                 manager.drawTotalWeight(totalWeight);
//             }
//             // FPS
//             int fps = GetFPS();
//             DrawText(TextFormat("FPS: %d", fps), 10, 10, 20, GREEN); 

//             // RANDOM PRUNING

//             float totalSum = 0.0f;
//             for (const auto& wor : connectionMatrix) {
//                 totalSum = std::accumulate(wor.begin(), wor.end(), totalSum);
//             }
//             totalWeight[(epoch)%500] = totalSum;
// // std::cout << totalSum << std::endl;

//     //----------------------------RESERVOIR UPDATE-----------------------------------
//             for(int sample = 0; sample < SPIKE_SAMPLING; sample++) {
//                 frameCounter++;
//     //----------------------------REFRACTORY PERIOD------------------------------
//                 for (Neuron* obj : disableBuffer[currentFrameIndex]) {
//                     obj->active = true;
//                     obj->color = WHITE;
//                 }
//                 disableBuffer[currentFrameIndex].clear(); // Reset the slot
//                 // Advance the ring buffer index
//                 currentFrameIndex = (currentFrameIndex + 1) % COOLDOWN_FRAMES;
//     //------------------------------RANDOM RESTRUCTURING---------------------------
//                 if (restructure) {
//                     for (int restruct = 0; restruct < 1000; restruct++) {
//                         toRemove = disreal(gen);
//                         if (!neurons[toRemove].receivers.empty()) {
//                             // get the ID of the neuron we disconnect from
//                             fromRemove = getRandomInt(neurons[toRemove].receivers.size()); // Get connection index 
//                             Neuron* targetNeuron = neurons[toRemove].receivers[fromRemove].first;
//                             fromRemove = targetNeuron->ID;
//                             // disconnect and get connection strength
//                             toDistribute = neurons[toRemove].disconnect(fromRemove);

//                             // if a connection has been removed, distribute its strength
//                             int sjdfo = static_cast<int>(toDistribute); 
//                             for (int a = 0; a < sjdfo; a++) {
//                                 if (dis(gen) < growthProb) {
//                                     float weight = static_cast<float>(SIZE);
//                                     auto [row, col] = manager.selectWeightedRandom(connectionMatrix, totalSum);
//                                     connectionMatrix[row][col] += 1.0f;
//                                     // connectionMatrix[toRemove][] += 1.0f;
//                                 } else {
//                                     from = disreal(gen);
//                                     to = disreal(gen);
//                                     if (connectionMatrix[from][to] != 0.0f){
//                                         connectionMatrix[from][to] += 1.0f;
//                                     } else {
//                                         neurons[from].connect(to, 1.0f);
//                                     }
//                                 }
//                             }
//                         }
//                         toDistribute = 0.0f;
//                     }
//                     restructure = false; 
//                 }
//     //-------------------------------INPUT----------------------------------------------
//                 // we queue N neurons to spike next
//                 if (sample%2==0) {
//                     scheduler.toSpike.insert(inputReservoir[epoch][0]);
//                     scheduler.toSpike.insert(inputReservoir[epoch][2]);
//                 } else {
//                     scheduler.toSpike.insert(inputReservoir[epoch][1]);
//                 } 
//                 //X(t) FOR THE MODEL
//                 if (train) {
//                     for (int ins : scheduler.toSpike){
//                         spikeHistory.push_back(ins+(SIZE*sample));
//                     }
//                 }
//                 //RESERVOIR
//                 spikeNumber[((epoch*10)+sample)%500] = scheduler.toSpike.size();
//                 scheduler.update();
//                 // scheduler.synaptoGenesis();
                
//                 // DECAY
//                 for (Neuron neur : neurons) {
//                     neur.actionPotential -= 6;
//                 }
//             }
//     //-----------------------------MODEL BY HAND-------------------------------
//             if (train) {
//                 Eigen::VectorXf output = network.forward_sparse(spikeHistory);
//                 output = network.softmax(output);
//                 epoch_loss += network.compute_loss(output, encodedTraining[epoch+1]);
//                 Eigen::VectorXf d_input = network.backward(spikeHistory, output, encodedTraining[epoch]); // 10000, 
//                 spikeHistory.clear();
//             }
//             EndDrawing();
//             epoch++;
//         }    
//         if (train) {
//         std::cout << "Epoch " << epoch/100 
//                 << " | Avg Loss: " << epoch_loss/100
//                 << std::endl;
//         }
//     }
//     CloseWindow();
//     return 0;
// }