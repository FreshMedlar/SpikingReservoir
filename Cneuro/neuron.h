#ifndef NEURON_H
#define NEURON_H

#include "manager.h"
#include "scheduler.h"
#include "raylib.h"
class Manager;
class Scheduler;

class Neuron {
public:
    Neuron( int id, 
            Manager& manager, 
            Scheduler& scheduler);
    void new_sender(Neuron* neuron);
    void connect(Neuron* neuron);    
    float x,y;
    std::vector<Neuron*> receiver; // neuron I send to
    void spike(Neuron* neuron);
    void impulse(Neuron* neuron);
    short actionPotential;
    Color color;

private:
    int ID;
    std::vector<Neuron*> sender;   // neuron send to me
    Manager& manager_;
    Scheduler& scheduler_;
};

#endif // NEURON_H
