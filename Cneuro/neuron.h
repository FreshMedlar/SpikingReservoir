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

    std::vector<std::pair<Neuron*, float>> receiver; // neuron I send to
    std::vector<Neuron*>    sender; // neuron send to me
    float                   x,y;
    short                   actionPotential;
    Color                   color;
    std::pair<int, int>     timer;
    int                     timeSinceSpike;
    int                     ID;

    // if nullptr is given in input it connect to a 
    // random neuron, otherwise to given neuron
    void connect(Neuron* neuron);   
    void disconnect(int n); 
    void spike(Neuron* neuron);
    void forward(int n);
    // update weight to next neuron (n)
    void backprop(int n);

private:
    Manager& manager_;
    Scheduler& scheduler_;
};

#endif // NEURON_H
