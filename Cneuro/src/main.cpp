#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <Eigen/Dense>
#include <cmath>
// #include <dlib/statistics.h>
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
#include "rl.h"

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
    manager.createNeurons();
    manager.initialConnections(100);
    // OR
    // manager.createSequentialNeurons();
    
    manager.status();
    manager.removeInputConnections(65);
    for (int dos = 0; dos < 100; dos++) {
        cout << senders[dos].size() << dos << " ";
    }
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
        for (int sos = 0; sos < 5; sos++) {randomIndexes.push_back(getRandomInt(0, 64));}

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
// #define DRAW
#ifndef DRAW
    InitWindow(screenWidth, screenHeight, "Raylib - Circle Manager");
    ToggleFullscreen();    
    SetTargetFPS(-1);
#endif
//----------------------------------DATA--------------------------------------------------
    // Eigen::MatrixXf spikeHistory = Eigen::MatrixXf::Zero(10, 1000); 
    vector<int> spikeHistory;
    spikeHistory.reserve(SPIKE_SAMPLING);
    vector<int> spikeNumber(200, 0); // Initialize with size 500 and default value 0


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
    float loss = 0.0f;
    float old_loss = 0.0f;
    vector<float> log_data(10, 1.0f);
    vector<float> nOfSpikes(SIZE, 0.0f);
    float result = 0.0f;
    int epoch = 0;
    float epoch_loss;
    static vector<int> fpsHistory;
    static long totalFPS = 0;
    std::cout << fileContent.size() << std::endl;
    std::cout << encodedTraining.size() << std::endl;
    vector<short> tracker;
    // while (!WindowShouldClose()) {
//--------------------------------------DEBUG VAR--------------------------------------------
    int countFail = 0;
    int toChange = 0;
    int change;
    float omegaChange;
    float alphaChange;
    float RL_loss = 0.0f;
    int inactive = 0;
    int past_inactive = 0;
    float generalImpulseChange = 0.0f;
    
    for (int letter = 0; letter < encodedTraining.size()-1; letter++) {
        // tracker.push_back(encodedTraining[letter]);
        for (int cycle = 0; cycle < 10; cycle++) {
    //------------------------------ NEURONS DRAWING ------------------------------------ 
#ifndef DRAW
                BeginDrawing();
                ClearBackground(BLACK);
                
                //DRAW
                // manager.draw();
                // manager.applyForces();
                //GRAPH
                connectionsPerNeuron.clear();
                connectionsPerNeuron.resize(SIZE, 0); 
                    // EITHER, NOT BOTH
                    manager.receiversFrequence(connectionsPerNeuron.data()); 
                    // manager.sendersFrequence(connectionsPerNeuron.data());
                manager.drawreceiversGraph(connectionsPerNeuron); // Draw the plot
                // manager.clustering();
                // SPIKES
                manager.drawSpikesGraph(spikeNumber);
                manager.drawTotalWeight();
                manager.drawOrder();
                manager.drawChaos();
                std::vector<float> sorted = frequency;  // Copy the original vector
                std::sort(sorted.begin(), sorted.end());  // Sort in ascending order
                manager.drawSpikeFrequencyDistribution(sorted);

                // cout << excitability[1000] << endl;

                // FPS  
                int fps = GetFPS();
                DrawText(TextFormat("FPS: %d", fps), 10, 10, 20, GREEN); 

                EndDrawing();
                
                fpsHistory.push_back(fps);
                totalFPS += fps;
                scheduler.updateColor();
#endif

        //-------------------------------INPUT----------------------------------------------
                //X(t) FOR THE MODEL
                // if (epoch % 10 == 9) {
                //     for (auto ins : spikeBuffer[currentSpikeIndex]){
                //         spikeHistory.push_back(ins+(SIZE*(epoch)));
                //     }
                // }

                //RESERVOIR
                // totalSum += spikeBuffer[currentSpikeIndex].size();
                spikeNumber[letter%200] = spikeBuffer[currentSpikeIndex].size(); // does not include current input
                for (short neur : spikeBuffer[currentSpikeIndex]) {
                    nOfSpikes[neur] += 1.0f; }
                scheduler.step(encodedTraining[letter]);
                // scheduler.pruningAndDecay();
                // scheduler.synaptoGenesis();

            // }
    //-----------------------------MODEL BY HAND-------------------------------
#define TRAIN
#ifndef TRAIN
            if (cycle == 9) {
#define RL
#ifndef RL
                if (letter%100 == 99) {
                    toChange = getRandomInt(0, 2);
                    int change = changeRandomInt(toChange);
                    // OR
                    float change = changeRandomFloat(toChange);
                }
#endif

                // cout << spikeBuffer[currentSpikeIndex].size() << endl;
                short target = encodedTraining[letter+1];
                Eigen::VectorXf output = network.forward_sparse(spikeBuffer[currentSpikeIndex]);
                output = network.softmax(output);
                loss = network.compute_loss(output, target);
                epoch_loss += loss;
                RL_loss += loss;
                Eigen::VectorXf d_input = network.backward(spikeBuffer[currentSpikeIndex], output, target); // 10000, 
                spikeHistory.clear();
#ifndef RL
                if (letter%100 == 99) {
                    // -----------------------
                    // if (RL_loss/100 > old_loss) { // reverse change if worse
                    // -------------------
                    int inactive = 0;
                    for (int i = 0; i < SIZE; i++) {
                        if (frequency[i] == 0) {inactive++;}
                        // OR
                        // inactive += frequency[i];
                        frequency[i] = 0;
                    }
                    cout << "TOTAL SPIKES -- " << inactive << endl;
                    if (inactive > past_inactive) {
                    // --------------------------
                        countFail += 1;
                        switch(toChange) {
                            case 0:
                                maxConnectionStrenght -= change;
                            break;
                            case 1:
                                generalThreshold -= change;
                            break;
                            case 2:
                                generalBias -= change;
                            break;
                            case 3:
                                omega -= omegaChange;
                                // if (generalRefractPeriod > 1) {
                                // generalRefractPeriod -= change;}
                            break;
                            case 4:
                                alpha -= alphaChange;
                            break;
                            case 5:
                                generalImpulse -= generalImpulseChange;
                            break;
                        }
                    }
                    

                    past_inactive = inactive;
                    inactive = 0;
                    old_loss = RL_loss/100;
                    RL_loss = 0;
                }
#endif

                if (letter%1000 == 0) {
                    cout << "Epoch " << letter+1
                            << " | Avg Loss: " << epoch_loss/1000
                            << endl;
                    epoch_loss = 0;
                    #ifndef RL
                    cout << countFail << endl;
                    countFail = 0;
                    cout << "maxConnectionStrenght " << maxConnectionStrenght << endl;
                    cout << "generalThreshold " << generalThreshold << endl;
                    cout << "generalBias " << generalBias << endl;
                    cout << "generalRefractPeriod " << generalRefractPeriod << endl;
                    cout << "omega " << omega << endl;
                    cout << "alpha " << alpha << endl;
                    cout << "Impulse " << generalImpulse << endl;
                    #endif
                }
                
                
                
            }
#endif

            epoch++;
        }
        // END LETTER
#define RL2
#ifndef RL2
        if (letter% 100 == 99) { // 99, 199...
            // std::sort(nOfSpikes, nOfSpikes + SIZE); 
            for (int i = 0; i<SIZE; i++) { nOfSpikes[i] = log(nOfSpikes[i]);}
            double score = anderson_darling_test(nOfSpikes);

            for (int i = 0; i<SIZE; i++) { nOfSpikes[i] = 0.0f;}
        }
#endif
    }
    // END


    cout << epoch << endl;
    // cout << decode(tracker, itos);

    
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


