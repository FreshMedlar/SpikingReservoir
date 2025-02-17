#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "neuron.h"
class Neuron;

class Scheduler {
public:
    Scheduler(int size);
    // std::vector<int> toAdd; 
    // toAdd contains the ID of the neurons 
    // that have just spiked 
    std::vector<int> toSpike; 
    // toSpike contains the ID of the neurons
    // that have reached the threshold and
    // will spike next iteration
    void update();
    
private:
    std::vector<std::vector<Neuron*>*> toReceive;
    int size;
    
};

#endif 