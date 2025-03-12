#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <Eigen/Dense>

#include "global.h"

using namespace std;

// The text to encode in input, in output a vector with the letters encoded in integers
vector<short> encode(string s, map<char, short> stoi) {
    vector<short> encoded;
    for (auto letters : s) {  encoded.push_back(stoi[letters]); }
    return encoded;
}

string decode(vector<short> i, map<short, char> itos) {
    string decoded;
    for (auto num : i) {decoded += itos[num];}
    return decoded;
}

Eigen::VectorXf vectorToEigen(const vector<int>& vec) {
    // Create an Eigen vector of the same size
    Eigen::VectorXf eigen_vec(vec.size());
    
    // Copy data from vector to Eigen::VectorXf
    for (size_t i = 0; i < vec.size(); ++i) {
        eigen_vec(i) = static_cast<float>(vec[i]);  // Cast int to float
    }
    
    return eigen_vec;
}

int getRandomInt(int n) {
    uniform_int_distribution<> randum(0, n-1);
    return randum(gen);
}

int getRandomFloat(float n) {
    uniform_real_distribution<> randfloat(0, n);
    return randfloat(gen);
}