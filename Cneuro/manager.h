#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include "neuron.h"

class Manager {
public:
    Manager(int size);
    int size;

    void createNeurons();
    std::vector<Neuron> neurons; 

    void status();

    void draw();
};

#endif 
