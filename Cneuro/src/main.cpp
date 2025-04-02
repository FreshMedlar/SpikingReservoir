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
#include "utilities.h"
#include "rl.h"
#include "simulation.h"

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
    vector<int> connectionsPerNeuron(1000, 0);
    uniform_real_distribution<> dis(0.0,1.0);
    uniform_int_distribution<> disreal(0, 1000-1);
//----------------------------------STUPID ENCODING------------------------------------------
    // from letters to integer
    vector<short> encodedTraining = encode(fileContent, stoi); 
    // vector<vector<vector<short>>> inputReservoir;
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


    Simulation sim(1000, 0.001, encodedTraining, true, true);

    sim.simulate();

    return 0;
}

































// ESSENTIAL

// #include <iostream>
// #include <algorithm>
// #include <vector>
// #include <fstream>
// #include <string>
// #include <set>
// #include <map>
// #include <Eigen/Dense>
// #include <cmath>

// #include "neuron.h"
// #include "manager.h"
// #include "raylib.h"
// #include "scheduler.h"
// #include "global.h"
// #include "net.h"
// #include "encoder.h"
// #include "utilities.h"
// #include "rl.h"

// // using namespace arma;
// using namespace std;

// struct Solution
// {
//     double rank, x, y, z;
//     void fitness (double p) {
//         rank = p - 1;
//     }
// };

// int main() {
// //-----------------------READ INPUT FILE----------------------
//     string filePath = "/home/medlar/SpikingReservoir/tinyshakespeare.txt";
//     ifstream file(filePath);
//     if (!file.is_open()) {
//         cerr << "Error opening file." << endl;
//         return 1;
//     }
//     string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
//     file.close();
// //-----------------------DATA PROCESSING-----------------------------
//     size_t textLength = fileContent.length();
//     set<char> uniqueCharsSet(fileContent.begin(), fileContent.end());
//     vector<char> uniqueChars(uniqueCharsSet.begin(), uniqueCharsSet.end());
//     vector<char> charVector(fileContent.begin(), fileContent.end());

//     // Example of translating characters to numbers
//     map<char, short> stoi;
//     map<short, char> itos;
//     for (short i = 0; i < uniqueChars.size(); i++) {
//         stoi[uniqueChars[i]] = i;
//         itos[i] = uniqueChars[i];
//     }
//     encodedTraining = encode(fileContent, stoi); 
// //----------------------------------RESERVOIR DEFINITION-----------------------------------
//     manager.createNeurons();
//     manager.initialConnections(100);
//     // OR
//     // manager.createSequentialNeurons();
    
//     manager.status();
//     // manager.removeInputConnections(65);

// //----------------------------------STUPID ENCODING------------------------------------------
    
//     // RANDOM SOLUTIONS
//     vector<Solution> solutions;
//     const int SOL = 100;
//     for (int i = 0; i < SOL; i++) {
//         solutions.push_back(Solution{
//             0, 
//             getRandomFloat(-10, 10),
//             getRandomFloat(0.0f, 0.5f),
//             getRandomFloat(2, 10),
//         });
//     }

//     //TEST THE SOLUTIONS
//     double p;
//     for (auto& s : solutions) {
//         omega = s.x;
//         alpha = s.y;
//         generalImpulse = s.z;
//         p = scheduler.simulation();
//         s.fitness(p);
//         cout << totalSum << endl;
//         totalSum = 0;
//     }

//     sort(solutions.begin(), solutions.end(), 
//     [](const auto& lhs, const auto& rhs){
//         return lhs.rank > rhs.rank;
//     });

//     for_each(
//         solutions.begin(),
//         solutions.begin() + 10, [](const auto& s) {
//             cout << fixed
//                 << "Rank " << static_cast<int>(s.rank) 
//                 << "\n x:" << s.x << " y:" << s.y << " z:" << s.z
//                 << "\n";
//         }
//     );


//     return 0;
// }
































// SINGLE

// #include <iostream>
// #include <algorithm>
// #include <vector>
// #include <fstream>
// #include <string>
// #include <set>
// #include <map>
// #include <Eigen/Dense>
// #include <cmath>
// // #include <dlib/statistics.h>
// // #include <ensmallen.hpp>
// // #include <cereal/archives/json.hpp>

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
// #include "rl.h"

// // using namespace arma;
// using namespace std;


// int main() {

//     manager.createSingle(0, false);
//     manager.createSingle(1, false);
    
//     connect(0, 1);
//     yA[1] += 10;
//     forward(0, 1);

//     for (int i = 0; i < 1000; i++) {
//         scheduler.step(0);
//         cout << "X0 " << xA[0] << ", Y0 " << yA[0] << endl;
//         cout << "X1 " << xA[1] << ", Y1 " << yA[1] << endl;
//     }

//     return 0;
// }
