#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include "neuron.h"

class Manager {
public:
    Manager(int size);
    void draw();
    void status();
    Neuron& connect (Neuron& n);

private:
    std::vector<Neuron> neurons; 
    int size;
    void createNeurons();

};

#endif 
