#ifndef NEURON_H
#define NEURON_H

#include "manager.h"
#include "scheduler.h"
#include "raylib.h"
#include <utility>
#include <stdint.h>

class Manager;
class Scheduler;

class Neuron {
public:
    Neuron( short id, 
            Manager& manager, 
            Scheduler& scheduler,
            uint8_t inhi);

    std::vector<std::pair<Neuron*, float>> receiver; // neuron I send to
    std::vector<Neuron*>    sender; // neuron send to me
    float                   x,y;
    short                   actionPotential;
    Color                   color;
    std::pair<int, int>     timer;
    int                     timeSinceSpike;
    short                   ID;
    uint8_t                 inhibitory;
    bool                    active;

    // if nullptr is given in input it connect to a 
    // random neuron, otherwise to given neuron
    void connect(short toConnect, float weight = 1.0f);   
    float disconnect(short n); 
    void spike(Neuron* neuron);
    void forward(short n, uint8_t inhi);
    // update weight to next neuron (n)
    void backprop(short n);
    void DisableObject();

private:
    Manager& manager_;
    Scheduler& scheduler_;
};

#endif // NEURON_H
