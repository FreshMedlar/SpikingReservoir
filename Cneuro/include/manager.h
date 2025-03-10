#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include "neuron.h"
#include "scheduler.h"
class Neuron;
class Scheduler;

class Manager {
public:
    Manager(int size);
    
    void countConnections(int* connections);
    // frequence of neurons with n outgoing connections 
    void receiversFrequence(int* connections);
    void sendersFrequence(int* connections);
    void draw();
    void drawreceiversGraph(const std::vector<int>& connectionsPerNeuron);
    void drawSpikesGraph(const std::vector<int>& spikeNumber);
    void drawTotalWeight(const std::vector<float>& totalWeight);
    void status();
    void applyForces();
    void initialConnections();
    void createNeurons();
    std::pair<std::size_t, std::size_t> selectWeightedRandom(const std::vector<std::vector<float>>& matrix, float totalSum);
    // return a random neuron
    Neuron* randomNeuron(Neuron* n);

private:
    int size;
    void updatePositions();
    static float calculateAttractionForce(float distance, float maxForce, float minDistance = 1.0f);
};

#endif 
