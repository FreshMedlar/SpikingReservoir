#include "neuron.h"
#include <random>
#include <iostream>
#include "global.h"

// Constructor definition
Neuron::Neuron(int id, Manager& manager, Scheduler& scheduler) : 
                ID(id), manager_(manager), scheduler_(scheduler) {
    std::uniform_real_distribution<> dis(0.0,1.0);
    x = 1920.0f*dis(gen);//460.0f + 1000.0f*dis(gen);
    y = 1080.0f*dis(gen);//250.0f + 580.0f*dis(gen); 
    actionPotential = 0;
    color = WHITE;
    timer = {ID, 60};
    timeSinceSpike = 0;
}

void Neuron::spike(Neuron* neuron) {
    // scheduler_.toAdd.push_back(ID);
    std::cout << "zoomba" << std::endl;
    for (std::pair<Neuron*, float> n: receiver) {n.first->forward(ID);}
    for (Neuron* n : sender) {n->backprop(ID);}
    std::cout << "half" << std::endl;
    timeSinceSpike = 0;
    color = RED;
    std::cout << "Timer: " << timer.first << ", " << timer.second << std::endl;
    std::cout << "Track color size: " << scheduler_.trackColor.size() << std::endl;
    scheduler_.trackColor.push_back(timer);
    std::cout << "psike" << std::endl;
}

void Neuron::forward(int n) {
    std::cout << "for" << std::endl;
    actionPotential += 30 * connectionMatrix[n][ID];
    if (actionPotential > 70) {
        actionPotential = 0;
        scheduler_.toSpike.push_back(ID);
    }
}

void Neuron::backprop(int n) {
    std::cout << "back" << std::endl;
    if (connectionMatrix[ID][n] > 0.8f) {
        if (connectionMatrix[ID][n] < 2.3f) {
            if (timeSinceSpike > 30) {
                // connectionMatrix[ID][n] -= 0.01f;
            } else {
                connectionMatrix[ID][n] += (-(timeSinceSpike-30)/100);
            }
        }
    } else {
        // disconnect(n);
    }
}

void Neuron::connect(Neuron* n) {
    if (n == nullptr) {
        std::pair<Neuron*, float> p = {manager_.randomConnection(*this), 1};
        receiver.push_back(p); 
    } else {
        std::pair<Neuron*, float> p = {n, 1};
        receiver.push_back(p);
    }
}

void Neuron::disconnect(int n) {
    // remove from sending (this) 
    for (int d = 0; d < receiver.size(); d++) {
        if (receiver[d].first == &neurons[n]) { 
            if ( d < receiver.size() - 1 )
                receiver[d] = std::move( receiver.back() );
            receiver.pop_back();
            break;
        }
    }
    // remove from receiving (next) 
    for (int d = 0; d < neurons[d].sender.size(); d++) {
        if (sender[d] == &neurons[n]) {
            if (d < sender.size() - 1) {
                sender[d] = std::move(sender.back());
            }
            sender.pop_back();
            break;
        }
    }
    //remove from connectionMatrix
    connectionMatrix[ID][n] = 0.0f;
}

void Neuron::new_sender(Neuron* n) {
    sender.push_back(n);
}
