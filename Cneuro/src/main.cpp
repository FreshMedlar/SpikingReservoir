#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <armadillo>
#include <ensmallen.hpp>
#include <cereal/archives/json.hpp>
// Define these to print extra informational output and warnings.
#define MLPACK_PRINT_INFO
#define MLPACK_PRINT_WARN
#include <mlpack/methods/ann/ann.hpp>
#include <mlpack/core.hpp>

#include "neuron.h"
#include "manager.h"
#include "raylib.h"
#include "scheduler.h"
#include "global.h"
#include "net.h"
#include "encoder.h"


using namespace mlpack::ann;
using namespace arma;
using namespace mlpack;
// using namespace mlpack::tree;
using namespace std;

int SIZE = 200;

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

int main() {
    std::string filePath = "/home/medlar/SpikingReservoir/tinyshakespeare.txt";
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }
    // Read the file content into a string
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    // Calculate the length of the text
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

    vector<int> sas = encode(training, stoi);
    vector<double> encodedData(sas.begin(), sas.end());
    sas.clear();
    mat encodedMatrix(encodedData.data(), encodedData.size(), 1);

    std::cout << "Dimensions: " << encodedMatrix.n_rows << " x " << encodedMatrix.n_cols << std::endl;


//-----------------------------------MODEL DEFINITION----------------------------------------

    // Define input and output dimensions
    const int inputSize = 10000;
    const int outputSize = 65;
    // Create a neural network model
    FFN<MeanSquaredError<>> model;
    // Add layers
    model.Add<Linear<>>(inputSize, 512); // First hidden layer (512 neurons)
    model.Add<ReLU<>>();                 // Activation function
    model.Add<Linear<>>(512, 256);       // Second hidden layer
    model.Add<ReLU<>>();                 
    model.Add<Linear<>>(256, outputSize);// Output layer
    // Set up the optimizer (Adam)
    ens::Adam optimizer(
        0.001,  // Learning rate
        32,     // Batch size
        0.9,    // Beta1
        0.999,  // Beta2
        1e-8,   // Epsilon
        10000,  // Max iterations
        1e-5,   // Tolerance
        false   // Shuffle
    );
    // Print model summary
    model.Print();

//----------------------------------RESERVOIR DEFINITION-----------------------------------
    Manager manager(SIZE);
    Scheduler scheduler(SIZE);
    manager.createNeurons(&scheduler);
    manager.initialConnections();
    manager.status();
    
    int frameCounter = 0;
    std::vector<int> connectionsPerNeuron(SIZE, 0);
    std::uniform_real_distribution<> dis(0.0,1.0);
    std::uniform_real_distribution<> disreal(0, SIZE-1);
//----------------------------------DATA--------------------------------------------------
    vector<vector<int>> spikeHistory;//TODO preassign memory
//----------------------------------TRAINING LOOP-----------------------------------------
    for(auto& batch: data_loader) {
        // the reservoir updated multiple times(spikeSampling) for each input 
        for(int sample = 0; sample < spikeSampling; sample++) {
            frameCounter++;
            
            //INPUT
            // scheduler.toSpike.push_back(next input)
            //X(t) FOR THE MODEL
            spikeHistory.push_back(scheduler.toSpike)
            //RESERVOIR
            scheduler.update();
            scheduler.synaptoGenesis();
        }
        //MODEL
        //do by hand

    }
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
//         // scheduler.changeColor();
//         manager.draw();
//         // if (frameCounter%2 ==0) {manager.applyForces();}

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