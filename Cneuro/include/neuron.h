#ifndef NEURON_H
#define NEURON_H

#include "raylib.h"
#include <utility>
#include <stdint.h>
#include <vector>

struct Neuron {
    short                   ID;
};

extern float biases[];
extern std::vector<std::vector<short>> receivers; // neuron I send to
extern std::vector<std::vector<short>> senders; // neuron send to me
extern Color colors[];
extern int timeSinceSpike[];
extern bool active[];
extern short actionPotential[];
extern short inhibitory[];
extern float xCoord[];
extern float yCoord[];
extern float xA[];
extern float yA[];

extern float threshold[];
extern std::vector<float> frequency;

extern int maxConnectionStrenght;
extern int generalThreshold;
extern int generalBias;
extern int generalRefractPeriod;

void constructorNeuron(Neuron& pre, short id, short inhi);
void colorNeuron(short pre, short time = 5);
void queueNeuron(short pre);
    // if nullptr is given in input it connect to a 
    // random neuron, otherwise to given neuron
void connect(short pre, short toConnect, float weight = 1.0f);   
void spike(short pre);
// disconnect pre from post
float disconnect(short pre, short post); 
void backprop(short from, short to, float delta);
void forward(short from, short to);

float sigmoid(float x);

#endif // NEURON_H
