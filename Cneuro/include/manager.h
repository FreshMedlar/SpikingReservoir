#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include "neuron.h"
#include "scheduler.h"
#include "utilities.h"

class Neuron;
class Scheduler;

class Manager {
public:
    Manager(int size);
    
    // frequence of neurons with n outgoing connections 
    void status();
    void initialConnections(int nConns = 10);
    void removeInputConnections(short nInput);
    void createNeurons();
    void createSequentialNeurons();
    void createSingle(short id, bool inhibitory);
    void connectSingle(short id, int nConns = 10);
    short randomNeuron(Neuron* n);
    
    void receiversFrequence(int* connections);
    void sendersFrequence(int* connections);
    void countConnections(int* connections);
    void applyForces();
    void clustering();
    
    void draw();
    void drawreceiversGraph(const std::vector<int>& connectionsPerNeuron);
    void drawSpikesGraph(std::vector<int>& spikeNumber);
    void drawTotalWeight();
    void drawOrder();
    void drawChaos();
    void drawSpikeFrequencyDistribution(vector<float> freq);
    std::pair<std::size_t, std::size_t> selectWeightedRandom(const std::vector<std::vector<float>>& matrix, float totalSum);
    // return a random neuron
    
    void saveModel();


private:
    int size;
    void updatePositions();
    static float calculateAttractionForce(float distance, float maxForce, float minDistance = 1.0f);
};

#endif 
