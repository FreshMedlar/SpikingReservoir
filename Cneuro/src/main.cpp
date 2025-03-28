#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <Eigen/Dense>
#include <cmath>

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

    //TEST THE SOLUTIONS
    double p;
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


