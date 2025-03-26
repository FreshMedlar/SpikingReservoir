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

struct Solution
{
    double rank, x, y, z;
    void fitness (double p) {
        rank = p - 1;
    }
};

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
    // for (auto i : decoded) {
    //     cout << i << " ";
    // }
    // for (char c : uniqueChars) {
    //     cout << stoi[c] << "";
    // }
    cout << endl;
//----------------------------------RESERVOIR DEFINITION-----------------------------------
    manager.createNeurons();
    manager.initialConnections(100);
    // OR
    // manager.createSequentialNeurons();
    
    manager.status();
    manager.removeInputConnections(65);
    // for (int dos = 0; dos < 100; dos++) {
    //     cout << senders[dos].size() << dos << " ";
    // }
    int SPIKE_SAMPLING = 10;
    int frameCounter = 0;
    vector<int> connectionsPerNeuron(SIZE, 0);
    uniform_real_distribution<> dis(0.0,1.0);
    uniform_int_distribution<> disreal(0, SIZE-1);
//----------------------------------STUPID ENCODING------------------------------------------
    // from letters to integer
    encodedTraining = encode(fileContent, stoi); 
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

    // vector<short> encodedUniqueChars;
    // for (char letter : uniqueChars){
    //     string s = {letter};
    //     encodedUniqueChars.push_back(encode(s, stoi)[0]);
    // }
    // vector<vector<vector<short>>> spikeEncodedChars (encodedUniqueChars.size());
    // for (int s = 0; s < encodedUniqueChars.size(); s++) {
    //     vector<int> randomIndexes;
    //     // take 5 random neurons for this letter
    //     for (int sos = 0; sos < 5; sos++) {randomIndexes.push_back(getRandomInt(0, 64));}

    //     for (short o = 0; o < SPIKE_SAMPLING; o++){
    //         vector<short> innerVec;
    //         innerVec.push_back(randomIndexes[o%5]);
    //         spikeEncodedChars[s].push_back(innerVec);
    //         innerVec.clear();
    //     }
    // }
    // for (short letter : encodedTraining) {
    //     inputReservoir.push_back(spikeEncodedChars[letter]);
    // }
//----------------------------------BY HAND MODEL------------------------------------------
    const int INPUT_SIZE = SIZE*10;
    const int OUTPUT_SIZE = 65;
    const float LR_M = 0.001;
    const int NUM_SAMPLES = 1;  // Online learning

    SingleLayerNetwork network(LR_M, SIZE);
//----------------------------------VISUALS-----------------------------------------------
#define DRAW
#ifndef DRAW
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Raylib - Circle Manager");
    ToggleFullscreen();    
    SetTargetFPS(-1);
#endif
    
    // RANDOM SOLUTIONS
    vector<Solution> solutions;
    const int SOL = 100;
    for (int i = 0; i < SOL; i++) {
        solutions.push_back(Solution{
            0, 
            getRandomFloat(-10, 10),
            getRandomFloat(0.0f, 0.5f),
            getRandomFloat(2, 10),
        });
    }

    double p;
    //TEST THE SOLUTIONS
    for (auto& s : solutions) {
        omega = s.x;
        alpha = s.y;
        generalImpulse = s.z;
        p = scheduler.simulation();
        s.fitness(p);
        cout << totalSum << endl;
        totalSum = 0;
    }

    sort(solutions.begin(), solutions.end(), 
    [](const auto& lhs, const auto& rhs){
        return lhs.rank > rhs.rank;
    });

    for_each(
        solutions.begin(),
        solutions.begin() + 10, [](const auto& s) {
            cout << fixed
                << "Rank " << static_cast<int>(s.rank) 
                << "\n x:" << s.x << " y:" << s.y << " z:" << s.z
                << "\n";
        }
    );


    return 0;
}


