#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "neuron.h"
#include <utility>
#include <algorithm> 
#include <set>
#include <Eigen/Dense>
#include "net.h"

class Neuron;

class Scheduler {
public:
    Scheduler(int size);
    // std::vector<int> toAdd; 
    // toAdd contains the ID of the neurons 
    // that have just spiked 
    std::set<short> toSpike; 
    std::set<short> swapSpike;
    // toSpike contains the ID of the neurons
    // that have reached the threshold and
    // will spike next iteration
    void update();
    void updateColor();
    // static std::vector<int> lonelyNeurons;
    // void synaptoGenesis();
    //
    Neuron& drawNeuron();
    void step(short letter);
    void pruningAndDecay();
    double simulation();

private:
    int size;
    
};

#endif 