#include "utilities.h"
#include "global.h"

#include <cmath>

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

// Returns in range [min, max]
int getRandomInt(int min, int max) {
    uniform_int_distribution<> randum(min, max);
    return randum(gen);
}

float getRandomFloat(float min, float max) {
    uniform_real_distribution<> randfloat(min, max);
    return randfloat(gen);
}

float shapiro_wilk_test(std::vector<float>& data) {
    int n = data.size();
    // if (n < 3 || n > 50) {
    //     std::cerr << "Shapiro-Wilk test is best for 3 ≤ n ≤ 50.\n";
    //     return -1;
    // }

    // Convert std::vector to Eigen::VectorXf
    Eigen::VectorXf sorted_data = Eigen::Map<const Eigen::VectorXf>(data.data(), n);

    // Step 2: Compute expected normal order statistics
    Eigen::VectorXf a(n);
    for (int i = 0; i < n; ++i) {
        float p = (i + 1 - 0.375f) / (n + 0.25f);
        a(i) = erfinv(2 * p - 1) * std::sqrt(2);
    }
    a /= a.norm();  // Normalize

    // Step 3: Compute W statistic
    float mean = sorted_data.mean();
    float numerator = std::pow(a.dot(sorted_data), 2);
    float denominator = (sorted_data.array() - mean).square().sum();

    float W = numerator / denominator;
    return W;
}

// Approximate inverse error function (erfinv)
double erfinv(double x) {
    double a = 0.147;
    double ln1mx2 = std::log(1 - x * x);
    double term1 = 2 / (M_PI * a) + ln1mx2 / 2;
    double term2 = ln1mx2 / a;
    return std::copysign(std::sqrt(std::sqrt(term1 * term1 - term2) - term1), x);
}

double anderson_darling_test(const float data[]) {
    std::vector<float> sorted_data(data, data + SIZE);
    std::sort(sorted_data.begin(), sorted_data.end());

    Eigen::VectorXf X = Eigen::Map<Eigen::VectorXf>(sorted_data.data(), SIZE);
    float mean = X.mean();
    float stddev = std::sqrt((X.array() - mean).square().sum() / SIZE);

    Eigen::VectorXf Z = (X.array() - mean) / stddev;  // Standardize
    Eigen::VectorXf F = 0.5 * (1 + Z.array().unaryExpr([](float x) { return std::erf(x); }));

    double A2 = -SIZE;
    for (size_t i = 0; i < SIZE; i++) {
        A2 -= (2 * (i + 1) - 1) * (std::log(F(i)) + std::log(1 - F(SIZE - i - 1)));
    }
    A2 /= SIZE;
    return A2;
}