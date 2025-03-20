#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <Eigen/Dense>
// #include <ensmallen.hpp>
// #include <cereal/archives/json.hpp>

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
#include "utilities.h"

// using namespace arma;
using namespace std;

int main() {
//-----------------------READ INPUT FILE----------------------
    string filePath = "/home/medlar/SpikingReservoir/tinyshakespeare.txt";
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }
    string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();
//-----------------------DATA PROCESSING-----------------------------
    size_t textLength = fileContent.length();
    cout << "Length of the text: " << textLength << endl;

    set<char> uniqueCharsSet(fileContent.begin(), fileContent.end());
    vector<char> uniqueChars(uniqueCharsSet.begin(), uniqueCharsSet.end());
    vector<char> charVector(fileContent.begin(), fileContent.end());

    int splitIndex = static_cast<int>(round(0.9*textLength));
    string training = fileContent.substr(0, splitIndex);
    string validation = fileContent.substr(splitIndex);
    // Print the vector of characters (optional)
    cout << "Characters in the vector: ";
    for (char c : uniqueChars) {
        cout << c;
    }
    cout << endl;

    // Example of translating characters to numbers
    map<char, short> stoi;
    map<short, char> itos;
    for (short i = 0; i < uniqueChars.size(); i++) {
        stoi[uniqueChars[i]] = i;
        itos[i] = uniqueChars[i];
    }

    cout << "Characters translated to numbers: ";
    string prova = "prova";
    vector<short> encoded = encode(prova, stoi);
    string decoded = decode(encoded, itos);
    for (auto i : decoded) {
        cout << i << " ";
    }
    for (char c : uniqueChars) {
        cout << stoi[c] << "";
    }
    cout << endl;
//----------------------------------RESERVOIR DEFINITION-----------------------------------
    // manager.createNeurons();
    // manager.initialConnections();
    manager.createSequentialNeurons();
    manager.status();
    manager.removeInputConnections(-65);
    
    int SPIKE_SAMPLING = 10;
    int frameCounter = 0;
    vector<int> connectionsPerNeuron(SIZE, 0);
    uniform_real_distribution<> dis(0.0,1.0);
    uniform_int_distribution<> disreal(0, SIZE-1);
//----------------------------------STUPID ENCODING------------------------------------------
    // from letters to integer
    vector<short> encodedTraining = encode(fileContent, stoi); 
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
        vector<int> randomIndexes;
        // take 5 random neurons for this letter
        for (int sos = 0; sos < 5; sos++) {randomIndexes.push_back(getRandomInt(65));}

        for (short o = 0; o < SPIKE_SAMPLING; o++){
            vector<short> innerVec;
            innerVec.push_back(randomIndexes[o%5]);
            spikeEncodedChars[s].push_back(innerVec);
            innerVec.clear();
        }
    }
    for (short letter : encodedTraining) {
        inputReservoir.push_back(spikeEncodedChars[letter]);
    }
//----------------------------------BY HAND MODEL------------------------------------------
    const int INPUT_SIZE = SIZE*10;
    const int OUTPUT_SIZE = 65;
    const float LR_M = 0.001;
    const int NUM_SAMPLES = 1;  // Online learning

    SingleLayerNetwork network(LR_M, SIZE);
//----------------------------------VISUALS-----------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    // InitWindow(screenWidth, screenHeight, "Raylib - Circle Manager");
    // ToggleFullscreen();    
    // SetTargetFPS(-1);
//----------------------------------DATA--------------------------------------------------
    // Eigen::MatrixXf spikeHistory = Eigen::MatrixXf::Zero(10, 1000); 
    vector<int> spikeHistory;
    spikeHistory.reserve(SPIKE_SAMPLING);
    vector<int> spikeNumber(500, 0); // Initialize with size 500 and default value 0


    vector<float> totalWeight;
    totalWeight.reserve(SIZE);
    totalWeight.resize(SIZE);
//---------------------------------REGROWTH---------------------------------------------------
    float growthProb = 0.66f;
    int toRemove;
    int fromRemove;
    float toDistribute = 0.0f;
    int from;
    int to;
//----------------------------------TRAINING LOOP-----------------------------------------
    int epoch = 0;
    float epoch_loss;
    bool train = false;
    bool draw = true;
    bool graph = true;
    bool restructure = false;
    static vector<int> fpsHistory;
    static long totalFPS = 0;
    std::cout << fileContent.size() << std::endl;
    std::cout << encodedTraining.size() << std::endl;
    vector<short> tracker;
    // while (!WindowShouldClose()) {
    for (int letter = 0; letter < encodedTraining.size(); letter++) {
        tracker.push_back(encodedTraining[letter]);
        for (int cycle = 0; cycle < 10; cycle++) {
    //------------------------------ NEURONS DRAWING ------------------------------------ 
            // BeginDrawing();
            // ClearBackground(BLACK);
            
            // //DRAW
            // // manager.draw();
            // // manager.applyForces();
            // //GRAPH
            // connectionsPerNeuron.clear();
            // connectionsPerNeuron.resize(SIZE, 0); 
            //     // EITHER, NOT BOTH
            //     manager.receiversFrequence(connectionsPerNeuron.data()); 
            //     // manager.sendersFrequence(connectionsPerNeuron.data());
            // manager.drawreceiversGraph(connectionsPerNeuron); // Draw the plot
            // manager.clustering();
            // // SPIKES
            // manager.drawSpikesGraph(spikeNumber);
            // // totalWeight[(epoch)%500] = totalSum;
            // manager.drawTotalWeight();
            // // cout << excitability[1000] << endl;

            // // FPS  
            // int fps = GetFPS();
            // DrawText(TextFormat("FPS: %d", fps), 10, 10, 20, GREEN); 

            // EndDrawing();
            
            // fpsHistory.push_back(fps);
            // totalFPS += fps;


    //--------------------------------------------------------------------------------------------------------
    //----------------------------RESERVOIR UPDATE------------------------------------------------------------
    //--------------------------------------------------------------------------------------------------------
            // for(int sample = 0; sample < SPIKE_SAMPLING; sample++) {
        //------------------------ REFRACTORY PERIOD ---------------------------------------------------------

                scheduler.updateColor();

        //------------------------- RANDOM RESTRUCTURING-------------------------------------------------------

                // for (int restruct = 0; restruct < 10000; restruct++) {
                //     toRemove = disreal(gen);
                //     if (!receivers[toRemove].empty()) {
                //         // get the ID of the neuron we disconnect from
                //         fromRemove = getRandomInt(receivers[toRemove].size()); // Get connection index 
                //         Neuron* targetNeuron = receivers[toRemove][fromRemove];
                //         fromRemove = targetNeuron->ID;
                //         // disconnect and get connection strength
                //         toDistribute = disconnect(fromRemove, toRemove);
                //         // if a connection has been removed, distribute its strength
                //         int sjdfo = static_cast<int>(toDistribute); 
                //         for (int a = 0; a < sjdfo; a++) {
                //             if (dis(gen) < growthProb) {
                //                 float weight = static_cast<float>(SIZE);
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

    //-------------------------------INPUT----------------------------------------------
                // we queue N neurons to spike next
                short input = SIZE - encodedTraining[letter];
                // for (short input : inputReservoir[epoch/10][epoch%10]){
                    if (active[input] == true) {
                        spikeBuffer[currentSpikeIndex].push_back(input);
                        active[input] = false;
                    }
                    colorNeuron(input); 
                // }
                // //X(t) FOR THE MODEL
                // if (epoch % 10 == 9) {
                //     for (auto ins : spikeBuffer[currentSpikeIndex]){
                //         spikeHistory.push_back(ins+(SIZE*(epoch)));
                //     }
                // }
                //RESERVOIR
                spikeNumber[letter%100] = spikeBuffer[currentSpikeIndex].size();

                scheduler.update();
                // scheduler.pruningAndDecay();
                // scheduler.synaptoGenesis();

            // }
    //-----------------------------MODEL BY HAND-------------------------------
            if (cycle == 9) {
                // a.push_back(encodedTraining[letter]);

                // cout << spikeBuffer[currentSpikeIndex].size() << endl;
                short target = letter+1;
                Eigen::VectorXf output = network.forward_sparse(spikeBuffer[currentSpikeIndex]);
                output = network.softmax(output);
                epoch_loss += network.compute_loss(output, encodedTraining[target]);
                Eigen::VectorXf d_input = network.backward(spikeHistory, output, encodedTraining[target]); // 10000, 
                spikeHistory.clear();
                if (letter%1000 == 999) {
                    
                    // cout << decode(a, itos) << endl;
                    // a.clear();
                    cout << "Epoch " << letter+1
                            << " | Avg Loss: " << epoch_loss/1000
                            << endl;
                    epoch_loss = 0;
                }
            }
            epoch++;
        }
    }
    cout << epoch << endl;
    cout << decode(tracker, itos);

    
    // CloseWindow();

    // int averageFPS = totalFPS / fpsHistory.size();
    // cout << averageFPS << endl;
    // cout << decode(encodedTraining, itos) << endl;
    return 0;
}









// #include "matplotlibcpp.h"
// namespace plt = matplotlibcpp;
// int main() {
//     plt::plot({1,3,2,4});
//     plt::show();
// }









// #include <iostream>
// #include <algorithm>
// #include <vector>
// #include <fstream>
// #include <string>
// #include <set>
// #include <map>
// #include <Eigen/Dense>
// #include <ensmallen.hpp>
// #include <cereal/archives/json.hpp>

// // Define these to print extra informational output and warnings.
// // #define MLPACK_PRINT_INFO
// // #define MLPACK_PRINT_WARN

// #include "neuron.h"
// #include "manager.h"
// #include "raylib.h"
// #include "scheduler.h"
// #include "global.h"
// #include "net.h"
// #include "encoder.h"
// #include "utilities.h"

// // using namespace arma;
// using namespace std;

// int main() {
// //----------------------------------RESERVOIR DEFINITION-----------------------------------
//     manager.createSingle(0, false);
//     manager.createSingle(1, false);
//     manager.initialConnections(); // matrix init
//     connect(neurons[0], 1);
    
//     connectionMatrix[0][1] = 1.0f;
//     biases[0] = 1.0f;
//     biases[1] = 1.0f;
    
//     spikeBuffer[1].push_back(0);
//     spikeBuffer[3].push_back(1);
//     spikeBuffer[10].push_back(0);
//     spikeBuffer[13].push_back(1);
//     spikeBuffer[20].push_back(0);
//     spikeBuffer[23].push_back(1);
//     spikeBuffer[30].push_back(0);
//     spikeBuffer[33].push_back(1);
//     spikeBuffer[40].push_back(0);
//     spikeBuffer[43].push_back(1);
//     for (int step = 0; step < 100; step++) {
//         scheduler.update();
//     }
//     cout << connectionMatrix[0][1] << endl;

//     // z[0] = biases[0]
//     // z[1] = connectionMatrix[0][1] * active[0] + biases[1]
//     // lambda[0] = exp(sigma[0]*z[0]/TEMP)
//     // lambda[1] = exp(sigma[1]*z[1]/TEMP)
//     // sigma is 1 if active, -1 if not active
//     // fixed biases, weigth updated based on formula



// //     int SPIKE_SAMPLING = 10;
// //     int frameCounter = 0;
// //     vector<int> connectionsPerNeuron(SIZE, 0);
// //     uniform_real_distribution<> dis(0.0,1.0);
// //     uniform_int_distribution<> disreal(0, SIZE-1);
// // //----------------------------------TRAINING LOOP-----------------------------------------
// //     int epoch = 0;
// //     float epoch_loss = 0;

// //     for (int nun = 0; nun < 100; nun++) {
// // //------------------------ REFRACTORY PERIOD ---------------------------------------------------------

// //             for (short obj : disableBuffer[currentFrameIndex]) {
// //                 active[obj] = true;
// //                 colors[obj] = WHITE;
// //             }
// //             disableBuffer[currentFrameIndex].clear(); // Reset the slot
// //             // Advance the ring buffer index
// //             currentFrameIndex = (currentFrameIndex + 1) % COOLDOWN_FRAMES;

// // //-------------------------------INPUT----------------------------------------------
// //             // we queue N neurons to spike next
// //             // for (short ll : inputReservoir[epoch/10][epoch%10]){
// //             //     scheduler.toSpike.insert(ll);
// //             // }
// //             //X(t) FOR THE MODEL
  
// //             //RESERVOIR
// //             scheduler.update();

// //         epoch++;
// //     }
// //     CloseWindow();

//     return 0;
// }


