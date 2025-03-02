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
    void receiverFrequence(int* connections);
    void senderFrequence(int* connections);
    void draw();
    void drawReceiverGraph(std::vector<int> connectionsPerNeuron);
    void drawSpikesGraph(std::vector<int> spikeNumber);
    void status();
    void applyForces();
    void initialConnections();
    void createNeurons(Scheduler* sched);
    // return a random neuron
    Neuron* randomNeuron(Neuron* n);

private:
    int size;
    void updatePositions();
    static float calculateAttractionForce(float distance, float maxForce, float minDistance = 1.0f);
};

#endif 
