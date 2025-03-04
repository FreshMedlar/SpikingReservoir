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
            Scheduler& scheduler,
            float fren,
            int inhi);

    std::vector<std::pair<Neuron*, float>> receiver; // neuron I send to
    std::vector<Neuron*>    sender; // neuron send to me
    float                   x,y;
    short                   actionPotential;
    Color                   color;
    std::pair<int, int>     timer;
    int                     timeSinceSpike;
    int                     ID;
    float                   friendliness;
    int                     inhibitory;
    bool                    active;

    // if nullptr is given in input it connect to a 
    // random neuron, otherwise to given neuron
    void connect(int toConnect, float weight = 1.0f);   
    float disconnect(int n); 
    void spike(Neuron* neuron);
    void forward(int n, int inhi);
    // update weight to next neuron (n)
    void backprop(int n);
    void DisableObject();

private:
    Manager& manager_;
    Scheduler& scheduler_;
};

#endif // NEURON_H
