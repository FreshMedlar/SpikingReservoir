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

int SIZE = 1000;

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
    Encoder encoder(10);

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
    std::uniform_real_distribution<> disreal(0, SIZE-1);
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
//----------------------------------TRAINING LOOP-----------------------------------------
    int epoch = 0;
    while (!WindowShouldClose()) {
        float epoch_loss = 0;



        BeginDrawing();
        ClearBackground(BLACK);

        // NEURONS DRAWING 
        // scheduler.changeColor();
        manager.draw();
        // if (frameCounter%3 ==0) {manager.applyForces();}
        // GRAPHS DRAWING
        if (frameCounter%3 == 0) {
            connectionsPerNeuron.clear();
            connectionsPerNeuron.resize(SIZE, 0); 
            // EITHER, NOT BOTH
            manager.receiverFrequence(connectionsPerNeuron.data()); 
            // manager.senderFrequence(connectionsPerNeuron.data());
        }
        manager.drawReceiverGraph(connectionsPerNeuron); // Draw the plot
        
        // FPS
        int fps = GetFPS();
        DrawText(TextFormat("FPS: %d", fps), 10, 10, 20, GREEN); 



        // the reservoir updated multiple times(spikeSampling) for each input 
        for (int circle = 0; circle < 1; ++circle) {
            for(int sample = 0; sample < SPIKE_SAMPLING; sample++) {
                frameCounter++;
                //INPUT
                for(int ins : inputReservoir[epoch]) {
                    scheduler.toSpike.push_back(ins);
                }
                //X(t) FOR THE MODEL
                for (int ins : scheduler.toSpike){
                    spikeHistory.push_back(ins+(SIZE*sample));
                }
                //RESERVOIR
                scheduler.update();
                scheduler.synaptoGenesis();
            }
            //MODEL BY HAND
            Eigen::VectorXf output = network.forward_sparse(spikeHistory);
            output = network.softmax(output);
            epoch_loss += network.compute_loss(output, encodedTraining[epoch+1]);
            Eigen::VectorXf d_input = network.backward(spikeHistory, output, encodedTraining[epoch]); // 10000, 
            spikeHistory.clear();
        }
        std::cout << "Epoch " << epoch 
                 << " | Avg Loss: " << epoch_loss/1
                 << std::endl;

        EndDrawing();
        epoch++;
    }
    CloseWindow();
    return 0;
}

    
        

























// int ordered_main() {
//     Manager manager(SIZE);
//     Scheduler scheduler(SIZE);
//     manager.createNeurons(&scheduler);
//     manager.initialConnections();
//     manager.status();
    
//     int frameCounter = 0;
//     std::vector<int> connectionsPerNeuron(SIZE, 0);
//     std::uniform_real_distribution<> dis(0.0,1.0);
//     std::uniform_real_distribution<> disreal(0, SIZE-1);

//     for (size_t epoch = 1; epoch <= 1; ++epoch) {
//         size_t batch_index = 0;
//         // Iterate the data loader to yield batches from the dataset.
//         for (auto& batch : *data_loader) {
//             for (int sample = 0; sample < spikeSampling; sample++){
//                 framecounter++;

//                 // RESERVOIR
//                 scheduler.update();
//                 scheduler.synaptoGenesis();

//                 // SPIKE
//             }

//             // FEATURE MODEL
//             // Reset gradients.
//             optimizer.zero_grad();
//             // Execute the model on the input data.
//             torch::Tensor prediction = net->forward(batch.data);
//             // Compute a loss value to judge the prediction of our model.
//             torch::Tensor loss = torch::nll_loss(prediction, batch.target);
//             // Compute gradients of the loss w.r.t. the parameters of our model.
//             loss.backward();
//             // Update the parameters based on the calculated gradients.
//             optimizer.step();
//             // Output the loss and checkpoint every 100 batches.
//             if (++batch_index % 100 == 0) {
//                 std::cout << "Epoch: " << epoch << " | Batch: " << batch_index
//                         << " | Loss: " << loss.item<float>() << std::endl;
//                 // Serialize your model periodically as a checkpoint.
//                 torch::save(net, "net.pt");
//             }
//         }
//     }
// }
























// int main() {
//     Manager manager(SIZE);
//     Scheduler scheduler(SIZE);
//     manager.createNeurons(&scheduler);
//     manager.initialConnections();
//     manager.status();

//     const int screenWidth = 1920;
//     const int screenHeight = 1080;

//     InitWindow(screenWidth, screenHeight, "Raylib - Circle Manager");
//     ToggleFullscreen();    
//     SetTargetFPS(0);
//     int frameCounter = 0;
//     std::vector<int> connectionsPerNeuron(SIZE, 0);
//     // for (int i = 0; i < 1500; i++) {manager.applyForces();};

//     std::uniform_real_distribution<> dis(0.0,1.0);
//     std::uniform_real_distribution<> disreal(0, SIZE-1);
//     while (!WindowShouldClose()) {
//         // std::cout << "BEGIN" << std::endl;
//         frameCounter++;
//         BeginDrawing();
//         ClearBackground(BLACK); // Black background

//         // NEURONS DRAWING 
//         scheduler.changeColor();
//         manager.draw();
//         // if (frameCounter%3 ==0) {manager.applyForces();}

//         // GRAPHS DRAWING
//         if (frameCounter%2 == 0) {
//             connectionsPerNeuron.clear();
//             connectionsPerNeuron.resize(SIZE, 0); 
//             // EITHER, NOT BOTH
//             manager.receiverFrequence(connectionsPerNeuron.data()); 
//             // manager.senderFrequence(connectionsPerNeuron.data());
//         }
//         manager.drawReceiverGraph(connectionsPerNeuron); // Draw the plot

//         scheduler.update();
//         scheduler.synaptoGenesis();
//         if (dis(gen) > 0.9) {
//             neurons[disreal(gen)].spike(nullptr);
//         }

//         // FPS
//         int fps = GetFPS();
//         DrawText(TextFormat("FPS: %d", fps), 10, 10, 20, GREEN); 
        
//         EndDrawing();
//         // std::cout << "END" << std::endl;
//     }

//     CloseWindow();

//     return 0;
// }