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
    void countFrequence(int* connections);
    void draw();
    void drawGraph(std::vector<int> connectionsPerNeuron);
    void status();
    void applyForces();
    void initialConnections();
    void createNeurons(Scheduler* sched);
    // given a pre-synaptic neuron, creates a connection
    // to a random post-synaptic neuron
    Neuron* randomConnection (Neuron* n);

private:
    int size;
    void updatePositions();
    static float calculateAttractionForce(float distance, float maxForce, float minDistance = 1.0f);
};

#endif 
