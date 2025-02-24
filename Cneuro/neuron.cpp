#include "neuron.h"
#include <random>
#include <iostream>
#include "global.h"

// Constructor definition
Neuron::Neuron(int id, Manager& manager, Scheduler& scheduler, float fren) : 
                ID(id), manager_(manager), scheduler_(scheduler), friendliness(fren) {
    std::uniform_real_distribution<> dis(0.0,1.0);
    x = 1920.0f*dis(gen);//460.0f + 1000.0f*dis(gen);
    y = 1080.0f*dis(gen);//250.0f + 580.0f*dis(gen); 
    actionPotential = 0;
    color = WHITE;
    timer = {ID, 60};
    timeSinceSpike = 0;
}

void Neuron::spike(Neuron* neuron) {
    for (std::pair<Neuron*, float> n: receiver) {n.first->forward(ID);}
    for (Neuron* n : sender) {n->backprop(ID);}
    if (timeSinceSpike> 300){ scheduler_.lonelyNeurons.push_back(this);}
    timeSinceSpike = 0;
    color = RED;
    // std::cout << "Timer: " << timer.first << ", " << timer.second << std::endl;
    // std::cout << "Track color size: " << scheduler_.trackColor.size() << std::endl;
    scheduler_.trackColor.push_back(timer);
}

void Neuron::forward(int n) {
    actionPotential += 30 * connectionMatrix[n][ID];
    if (actionPotential > 70) {
        actionPotential = 0;
        scheduler_.nextSpike.push_back(ID);
    }
}

void Neuron::backprop(int n) {
    if (connectionMatrix[ID][n] > 0.9f) {
        if (connectionMatrix[ID][n] < 2.3f) {
            if (timeSinceSpike > 30) {
                connectionMatrix[ID][n] -= 0.01f;
            } else {
                connectionMatrix[ID][n] += (-(timeSinceSpike-10)/50);
            }
        }
    } else {
        disconnect(n);
    }
}

void Neuron::disconnect(int n) {
    // remove the receiving neuron 
    for (int d = 0; d < receiver.size(); d++) {
        if (receiver[d].first == &neurons[n]) { 
            if ( d < receiver.size() - 1 )
                receiver[d] = std::move( receiver.back() );
            receiver.pop_back();
            break;
        }
    }
    // remove this neuron from sending
    for (int jkl = 0; jkl < neurons[n].sender.size(); jkl++) {
        if (neurons[n].sender[jkl] == this) {
            if (jkl < neurons[n].sender.size() - 1) {
                neurons[n].sender[jkl] = std::move(neurons[n].sender.back());
            }
            neurons[n].sender.pop_back();
            break;
        }
    }
    //remove from connectionMatrix
    connectionMatrix[ID][n] = 0.0f;
}

void Neuron::connect(Neuron* n) {
    if (n == nullptr) {
        n = manager_.randomNeuron(this);
    }
    n->sender.push_back(this);
    std::pair<Neuron*, float> p = {n, 1};
    receiver.push_back(p); 
    connectionMatrix[ID][n->ID] = 1.0f;
}