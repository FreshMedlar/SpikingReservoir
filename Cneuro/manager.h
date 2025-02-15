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
    Neuron* trackConnection (Neuron& n);
    void applyForces();
    void initialConnections();
    void createNeurons(Scheduler* sched);

private:
    std::vector<std::vector<int>> connectionMatrix;
    int size;
    void updatePositions();
};

#endif 
