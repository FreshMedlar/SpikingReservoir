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

    manager.createSingle(0, false);
    manager.createSingle(1, false);
    cout << "prova" << endl;
    connect(0, 1);
    cout << "prova" << endl;
    forward(0, 1);
    for (int i = 0; i < 10; i++) {
        scheduler.step(0);
        // cout << "X1 " << xA[1] << ", Y1 " << yA[1] << endl;
    }

    return 0;
}


