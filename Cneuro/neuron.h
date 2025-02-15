#ifndef NEURON_H
#define NEURON_H

#include "manager.h"
#include "scheduler.h"
class Manager;
class Scheduler;

class Neuron {
public:
    Neuron(int id, Manager& manager, Scheduler& scheduler);
    void new_sender(Neuron* n);
    void connect(Neuron* n);    
    float x,y;
    std::vector<Neuron*> receiver; // neuron i send to
    void spike(Neuron* n);

private:
    int ID;
    std::vector<Neuron*> sender;   // neuron send to me
    Manager& manager_;
    Scheduler& scheduler_;
};

#endif // NEURON_H
