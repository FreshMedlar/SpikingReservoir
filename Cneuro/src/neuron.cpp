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
float excitability[SIZE];

int maxConnectionStrenght = 23;
int generalThreshold = 23;
int generalBias = 30;
int generalRefractPeriod = 3;
float omega = 0.11;
float alpha = 0.01;
float generalImpulse = 100.0f;

void constructorNeuron(Neuron& pre, short id, short inhi) {
    
    pre.ID = id;

    std::uniform_real_distribution<> dis(0.0,1.0);
    
    xA[id] =                0.0f;
    yA[id] =                0.0f;
    xCoord[id] =            1920.0f*dis(gen);
    yCoord[id] =            1080.0f*dis(gen);
    biases[id] =            getRandomFloat(0.0f, 5.0f);
    colors[id] =            WHITE;
    active[id] =            true;
    frequency[id] =         0.0f;
    inhibitory[id] =        inhi;
    threshold[id] =         30;
    excitability[id] =      1.0f;
    timeSinceSpike[id] =    1000; // to ignore the first spike
    actionPotential[id] =   0;
}

void spike(short pre) {
    // connection strenghten    if the previous neuron  just spiked
    // connection weaken        if the next neuron      just spiked
    
    cout << "Neuron " << pre << " spiked at Y " << yA[pre] << endl;
    // check if the neuron is in a self-loop state
    int next = whenSpike(pre);

        // float delta = inhibitory[pre]*LR/exp(timeSinceSpike[pre]/TEMP); //we calc now for efficiency
        for (short n: receivers[pre]) { if (active[n]) {forward(pre, n);}}
        // for (short prepre : senders[pre]) { backprop(prepre, pre, delta); }
    excitability[pre] += 0.1;
    timeSinceSpike[pre] = 0;
    frequency[pre] += 1.0f;
    cout << next << endl;
    if (next == 0) {
        active[pre] = true;
    } else {
        queueNeuron(pre, next);
    }
}   

void forward(short spiked, short post) {
    // w[spiked][post] depends on timeSinceSpike[post]
    // if timeSinceSpike[post] small, w go down :(
    
    // this act as a threshold, small perturbation will not make the attractor spike
    // TODO - integrate all inputs at the end of the step, to make bigger changes possible
    actionPotential[post] +=  (connectionMatrix[spiked][post]
                            * inhibitory[spiked]) + generalBias;
    
    if (actionPotential[post] > (generalThreshold*excitability[post])) {
        actionPotential[post] = 0;
        yA[post] += generalImpulse;
        queueNeuron(post, 1); // insert in next step
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

void queueNeuron(short pre, short next) {
    short targetSlot = (currentSpikeIndex + next) % SPIKE_BUFFER_SIZE; // was SPIKE_FRAMES
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

// return the current position of the point from last spike
pair<float, float> whereIs(short id) {
    float decay = exp(-alpha * timeSinceSpike[id]);
    float theta = atan(omega/(1 - alpha));

    float co = cos(timeSinceSpike[id] * theta);
    float si = sin(timeSinceSpike[id] * theta);
    float x = decay * (xA[id] * co - yA[id] * si);
    float y = decay * (yA[id] * co + xA[id] * si);
    return make_pair(x, y);
}

// returns the next time the neuron will spike
int whenSpike(short id) {
    cout << xA[id] << " " << yA[id] << endl;
    // ignore if point is close to basin
    if (xA[id]*xA[id] + yA[id]*yA[id] < 2) {
        return 0;
    }
    if (yA[id] != 0.0f) {
        return 0;
    }
    float nextSpike = (1.0f/omega) * atan(xA[id]/yA[id]);
    // If the timing is negative we take the next
    cout << "Next " << nextSpike << endl;
    if (nextSpike <= 0){
        nextSpike += M_PI / omega;
        cout << "PI " << M_PI/omega << endl;
    }
    return static_cast<int>(std::ceil(nextSpike));
}