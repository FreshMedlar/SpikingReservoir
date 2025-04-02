#ifndef SIMULATION_H
#define SIMULATION_H

#include <Eigen/Dense>
#include <vector>
#include <array>
#include "raylib.h"

#include "neuron.h"
#include "manager.h"
#include "scheduler.h"
#include "net.h"

using namespace std;

class Manager;

struct Simulation {
    const short SIZE;
    bool train;
    bool draw;
    Manager& manager;
    Scheduler& scheduler;
    SingleLayerNetwork& network;

    // SETTINGS
    vector<vector<float>> connectionMatrix; // row n is for outgoing connection of neuron n 
    vector<short> encodedTraining;
    short CYCLE_LEN = 50;
    float LR = 1; // magnitued of weights updated
    float TEMP = 3; // importance of timing (low TEMP, strict timing)
    
    // NEURONS
    vector<Neuron> neurons;
    vector<vector<short>> receivers; // neuron I send to
    vector<vector<short>> senders; // neuron send to me
    vector<Color> colors;
    vector<float> biases;
    vector<int> timeSinceSpike;
    vector<bool> active;
    vector<short> inhibitory;
    vector<float> xCoord;
    vector<float> yCoord;
    vector<float> xA;
    vector<float> yA;
    vector<float> excitability;
    vector<float> frequency;
    vector<float> threshold;

    // buffer for spikes
    static const short SPIKE_FRAMES = 3; 
    static const size_t SPIKE_BUFFER_SIZE = 10000;
    array<vector<short>, SPIKE_BUFFER_SIZE> spikeBuffer;
    int currentSpikeIndex;
    
    // color
    static const size_t COLOR_FRAMES = 5;
    array<vector<short>, COLOR_FRAMES> colorBuffer;
    int currentColorIndex; 
    

    // CONTROL VARIABLES
    // sum of weights
    float totalSum;
    // number of outgoing connections
    vector<int> connectionsPerNeuron;//SIZE, 0);
    // spikes per step
    vector<int> spikeNumber;

    
    Simulation(short size, 
        float LR, 
        vector<short> encodedTraining, 
        bool draw, 
        bool train);
    float simulate();
    float forward();

};



#endif // SIMULATION_H
