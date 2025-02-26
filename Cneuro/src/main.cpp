#include <iostream>
#include <algorithm>
#include <torch/torch.h>
#include <vector>
#include <fstream>
#include <string>
#include <set>
#include <map>

#include "neuron.h"
#include "manager.h"
#include "raylib.h"
#include "scheduler.h"
#include "global.h"
#include "net.h"
#include "encoder.h"

using namespace torch;

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

    auto options =
        torch::TensorOptions()
            .dtype(torch::kFloat32);
    torch::Tensor data = torch::tensor(encode(fileContent, stoi), options);
    std::cout << data.dtype() << std::endl << data.sizes() << std::endl;

    
    return 0;
}

// int ordered_main() {
//     Manager manager(SIZE);
//     Scheduler scheduler(SIZE);
//     manager.createNeurons(&scheduler);
//     manager.initialConnections();
//     manager.status();
//     auto net = std::make_shared<Net>(10, 10);
//     torch::optim::SGD optimizer(net->parameters(), /*lr=*/0.01);
    
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