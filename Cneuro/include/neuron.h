#ifndef NEURON_H
#define NEURON_H

#include "raylib.h"
#include <utility>
#include <stdint.h>
#include <vector>

struct Neuron {
    Color                   color;
    float                   x,y;
    int                     timeSinceSpike;
    short                   ID;
    short                   actionPotential;
    uint8_t                 inhibitory;
    bool                    active;
};

extern std::vector<std::vector<Neuron*>> receivers; // neuron I send to
extern std::vector<std::vector<Neuron*>> senders; // neuron send to me

void constructorNeuron(Neuron& pre, short id, uint8_t inhi);
void DisableObject(Neuron pre);
    // if nullptr is given in input it connect to a 
    // random neuron, otherwise to given neuron
void connect(Neuron& pre, short toConnect, float weight = 1.0f);   
void spike(Neuron& neuron);
// disconnect pre from post
float disconnect(short pre, short post); 
void backprop(short from, short to, int timeSinceSpike);
void forward(short from, short to, bool active, short actionPotential, uint8_t inhi);

#endif // NEURON_H
