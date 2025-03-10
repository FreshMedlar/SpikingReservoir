#ifndef NEURON_H
#define NEURON_H

#include "raylib.h"
#include <utility>
#include <stdint.h>
#include <vector>

struct Neuron {
    short                   ID;
};

extern std::vector<std::vector<Neuron*>> receivers; // neuron I send to
extern std::vector<std::vector<Neuron*>> senders; // neuron send to me
extern Color colors[];
extern int timeSinceSpike[];
extern bool active[];
extern int8_t actionPotential[];
extern int8_t inhibitory[];
extern float xCoord[];
extern float yCoord[];

void constructorNeuron(Neuron& pre, short id, uint8_t inhi);
void DisableObject(Neuron& pre);
    // if nullptr is given in input it connect to a 
    // random neuron, otherwise to given neuron
void connect(Neuron& pre, short toConnect, float weight = 1.0f);   
void spike(int neur);
// disconnect pre from post
float disconnect(short pre, short post); 
void backprop(short from, short to);
void forward(short from, short to);

#endif // NEURON_H
