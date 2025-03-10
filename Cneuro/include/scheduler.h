#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "neuron.h"
#include <utility>
#include <algorithm> 
// #include <thread>
// #include <atomic>
#include <set>

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
    // static std::vector<int> lonelyNeurons;
    // void synaptoGenesis();
    Neuron& drawNeuron();
    // std::vector<std::thread> threads;
    // static void parallel_push_back(int value);

private:
    int size;
    
};

#endif 