#ifndef NEURON_H
#define NEURON_H

#include "manager.h"
#include "scheduler.h"
#include "raylib.h"
#include <utility>

class Manager;
class Scheduler;

class Neuron {
public:
    Neuron( int id, 
            Manager& manager, 
            Scheduler& scheduler);

    float                   x,y;
    // neuron I send to
    std::vector<std::pair<Neuron*, float>> receiver; 
    short                   actionPotential;
    Color                   color;
    std::pair<int, int>     timer;

    void new_sender(Neuron* neuron);
    // if nullptr is given in input it connect to a 
    // random neuron, otherwise to given neuron
    void connect(Neuron* neuron);    
    void spike(Neuron* neuron);
    void forward(int n);
    void backprop(int n);
    int timeSinceSpike;

private:
    int ID;
    std::vector<Neuron*> sender;   // neuron send to me
    Manager& manager_;
    Scheduler& scheduler_;
};

#endif // NEURON_H
