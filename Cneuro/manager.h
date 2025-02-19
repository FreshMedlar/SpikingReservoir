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
    void draw();
    void status();
    // given a pre-synaptic neuron, creates a connection
    // to a random post-synaptic neuron
    Neuron* randomConnection (Neuron& n);

    void applyForces();
    void initialConnections();
    void createNeurons(Scheduler* sched);
    int countNonZero(std::vector<std::vector<float>>& matrix);

private:
    int size;
    void updatePositions();
    static float calculateAttractionForce(float distance, float maxForce, float minDistance = 1.0f);
};

#endif 
