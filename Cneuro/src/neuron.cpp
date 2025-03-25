#include "neuron.h"
#include <random>
#include <iostream>
#include "global.h"
#include <cmath>

float biases[SIZE];
std::vector<std::vector<short>> senders(SIZE); // neurons that send to me
std::vector<std::vector<short>> receivers(SIZE); // neuron I send to
Color colors[SIZE];
int timeSinceSpike[SIZE];
bool active[SIZE];
short actionPotential[SIZE];
short inhibitory[SIZE];
float xCoord[SIZE];
float yCoord[SIZE];
float threshold[SIZE];
float xA[SIZE];
float yA[SIZE];
std::vector<float> frequency(SIZE);

int maxConnectionStrenght = 15;
int generalThreshold = 30;
int generalBias = 5;
int generalRefractPeriod = 3;

void constructorNeuron(Neuron& pre, short id, short inhi) {
    pre.ID = id;

    std::uniform_real_distribution<> dis(0.0,1.0);
    xA[id] =                0.0f;
    yA[id] =                0.0f;
    xCoord[id] =            1920.0f*dis(gen);
    yCoord[id] =            1080.0f*dis(gen);
    biases[id] =            getRandomFloat(5.0f);
    colors[id] =            WHITE;
    active[id] =            true;
    frequency[id] =         0.0f;
    inhibitory[id] =        inhi;
    threshold[id] =         30;
    timeSinceSpike[id] =    1000; // to ignore the first spike
    actionPotential[id] =   0;
}

void spike(short pre) {
    // connection strenghten    if the previous neuron  just spiked
    // connection weaken        if the next neuron      just spiked

    
        float delta = inhibitory[pre]*LR/exp(timeSinceSpike[pre]/TEMP); //we calc now for efficiency
        for (short n: receivers[pre]) { if (active[n]) {forward(pre, n);}}
        for (short prepre : senders[pre]) { backprop(prepre, pre, delta); }

        // if (timeSinceSpike> 1000){ scheduler_.lonelyNeurons.push_back(ID);}
    // threshold[pre] -= 3*sigmoid(timeSinceSpike[pre]);
    // threshold[pre] += threshold[pre]*(0.1f * (1.0 - threshold[pre]));
    // if (timeSinceSpike[pre] < 10) {threshold[pre]-=0.3;}
    timeSinceSpike[pre] = 0;
    frequency[pre] += 1.0f;

}

void forward(short spiked, short post) {
    // w[spiked][post] depends on timeSinceSpike[post]
    // if timeSinceSpike[post] small, w go down :(
    actionPotential[post] +=  (connectionMatrix[spiked][post]
                            * inhibitory[spiked]) + generalBias;
    
    if (actionPotential[post] > generalThreshold) {
        actionPotential[post] = 0;
        xA[post] += 2;
        queueNeuron(post);
        colorNeuron(post);

        // float delta = LR/exp(timeSinceSpike[post]/TEMP);
        // connectionMatrix[spiked][post] -= delta;
        // totalSum -= delta;
    
    } else {
        // threshold[post] += 1.0f;
    }
}
void backprop(short pre, short post, float delta) {
    // w[pre][post] depends on timeSinceSpike[pre] 
    // if timeSinceSpike[pre] small, w go up :)
    if (connectionMatrix[pre][post] < maxConnectionStrenght) {
        connectionMatrix[pre][post] += delta;
        totalSum += delta;
    }
}

float disconnect(short pre, short post) {
    // remove the receiving neuron 
    for (int d = 0; d < receivers[pre].size(); d++) {
        if (receivers[pre][d] == post) { 
            if ( d < receivers[pre].size() - 1 )
                receivers[pre][d] = std::move( receivers[pre].back() );
            receivers[pre].pop_back();
            break;
        }
    }
    // remove this neuron from sending
    for (int jkl = 0; jkl < senders[post].size(); jkl++) {
        if (senders[post][jkl] == pre) {
            if (jkl < senders[post].size() - 1) {
                senders[post][jkl] = std::move(senders[post].back());
            }
            senders[post].pop_back();
            break;
        }
    }
    //remove from connectionMatrix
    float strength = connectionMatrix[pre][post];
    totalSum -= strength;
    connectionMatrix[pre][post] = 0.0f;
    return strength;
}

void connect(short pre, short toConnect, float weight) {
    // Select a random neuron if toConnect is -1
    if (toConnect == -1) {
        toConnect = manager.randomNeuron(&neurons[pre]);
    }
    // Establish the connection
    senders[toConnect].push_back(pre);
    receivers[pre].push_back(toConnect);
    connectionMatrix[pre][toConnect] = weight;
    totalSum += weight;
}


void queueNeuron(short pre) {
    short targetSlot = (currentSpikeIndex + generalRefractPeriod) % SPIKE_BUFFER_SIZE; // was SPIKE_FRAMES
    spikeBuffer[targetSlot].push_back(pre);
    active[pre] = false;
}

void colorNeuron(short pre, short time) {
    int targetSlot = (currentColorIndex + time) % COLOR_FRAMES;
    colorBuffer[targetSlot].push_back(pre);
    colors[pre] = RED;
}

float sigmoid(float x) {
   return (3.0f/(1.0f+exp(-x)));
}

float sigmoid_derivative(float x) {
    float s = sigmoid(x);
    return s * (1.0f - s);
}