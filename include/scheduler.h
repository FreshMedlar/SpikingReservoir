#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "neuron.h"
#include "net.h"
#include "scheduler.h"
#include "simulation.h"
#include "raylib.h"
#include "global.h"

#include <vector>
#include <utility>
#include <algorithm> 
#include <set>
#include <eigen3/Eigen/Dense>
#include <iostream>
#include <numeric>
#include <random>
// #include <omp.h>





using namespace std;

struct Simulation;
struct Neuron;

class Scheduler {
public:
    Scheduler(int size, Simulation& simulation);
    // std::vector<int> toAdd; 
    // toAdd contains the ID of the neurons 
    // that have just spiked 
    set<short> toSpike; 
    set<short> swapSpike;
    // toSpike contains the ID of the neurons
    // that have reached the threshold and
    // will spike next iteration
    void update();
    void updateColor();
    // static std::vector<int> lonelyNeurons;
    // void synaptoGenesis();
    //
    Neuron& pickNeuron();
    void step(short letter);
    void pruningAndDecay();
    // double simul();

    
    private:
    // REFERENCES
    Simulation& simulation;
    vector<Neuron>& neurons;
    vector<vector<float>>& connectionMatrix;    
    int size;
    
};

#endif 