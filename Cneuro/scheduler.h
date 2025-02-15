#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "neuron.h"
class Neuron;

class Scheduler {
public:
    Scheduler();
    std::vector<std::vector<Neuron*>*> toReceive;
    
private:
    std::vector<Neuron> neurons; 
    std::vector<std::vector<int>> connectionMatrix;
    int size;
    
};

#endif 