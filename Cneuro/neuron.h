#ifndef NEURON_H
#define NEURON_H

#include "manager.h"

class Neuron {
public:
    Neuron(int id, Manager& manager);
    void new_sender(Neuron& n);

private:
    int ID;
    std::vector<Neuron> receiver; // neuron i send to
    std::vector<Neuron> sender;   // neuron send to me
    Manager& manager_;
    Neuron& connect();    
};

#endif // NEURON_H
