#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <set>
#include <map>
#include <Eigen/Dense>
#include <random>

using namespace std;

// The text to encode in input, in output a vector with the letters encoded in integers
vector<short> encode(string s, map<char, short> stoi);

string decode(vector<short> i, map<short, char> itos);

Eigen::VectorXf vectorToEigen(const vector<int>& vec);

int getRandomInt(int n);

float getRandomFloat(float n);

#endif 