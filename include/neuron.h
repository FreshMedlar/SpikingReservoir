#ifndef NEURON_H
#define NEURON_H

#include "raylib.h"
#include "simulation.h"
#include "manager.h"
#include "global.h"

#include <utility>
#include <stdint.h>
#include <vector>
#include <random>
#include <iostream>
#include <cmath>

using namespace std;

struct Simulation;

struct Neuron {
    Neuron(Simulation& simulation, short id, short inhi);
    Simulation& simulation;
    short ID;

    // REFERENCE FOR SIMPLICITY
    vector<float>& biases;
    vector<vector<short>>& receivers; // neuron I send to
    vector<vector<short>>& senders; // neuron send to me
    vector<Color>& colors;
    vector<int>& timeSinceSpike;
    vector<bool>& active;
    vector<short>& inhibitory;
    vector<float>& xCoord;
    vector<float>& yCoord;
    vector<float>& xA;
    vector<float>& yA;
    vector<float>& excitability;
    vector<float>& frequency;
    vector<float>& threshold;

    vector<vector<float>>& connectionMatrix;
    vector<Neuron>& neurons;
    Manager& manager;

    void constructorNeuron(Neuron& pre, short id, short inhi);
    void colorNeuron(short pre, short time = 5);
    void queueNeuron(short pre, short next = 3);
        // if nullptr is given in input it connect to a 
        // random neuron, otherwise to given neuron
    void connect(short pre, short toConnect, float weight = 1.0f);   
    void spike(short pre);
    // disconnect pre from post
    float disconnect(short pre, short post); 
    void backprop(short from, short to, float delta);
    void forward(short from, short to);
    
    float sigmoid(float x);
    pair<float, float> whereIs(short id, float time = -1.0f);
    int whenSpike(short id);
};


extern int maxConnectionStrenght;
extern float generalThreshold;
extern int generalRefractPeriod;
extern float omega;
extern float alpha;
extern float generalImpulse;



#endif // NEURON_H
