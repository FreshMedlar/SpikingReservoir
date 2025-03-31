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
short inhibitory[SIZE];
float xCoord[SIZE];
float yCoord[SIZE];
float threshold[SIZE];
float xA[SIZE];
float yA[SIZE];
std::vector<float> frequency(SIZE);
float excitability[SIZE];

int maxConnectionStrenght = 23;
float generalThreshold = 2.0f;
float omega = 0.11;
float alpha = 0.01;
float generalImpulse = 1.0f;

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
}

void spike(short pre) {
    frequency[pre] += 1.0f;
    // cout << "Neuron " << pre << " spiked at Y " << yA[pre] << endl;
    // connection strenghten    if the previous neuron  just spiked
    // connection weaken        if the next neuron      just spiked
    
    // time of next spike
    int next = whenSpike(pre);

        float delta = LR/exp(timeSinceSpike[pre]/TEMP); //we calc now for efficiency
        for (short n: receivers[pre]) { if (active[n]) {forward(pre, n);}}
        for (short prepre : senders[pre]) { backprop(prepre, pre, delta); }

    excitability[pre] += 2.0f;
    timeSinceSpike[pre] = 0;
    if (next == 0)  {   active[pre] = true;   
                        yA[pre] = 0.0f;
                        xA[pre] = 0.0f;         }
    else            {   queueNeuron(pre, next); }
}   

void forward(short spiked, short post) {
    // w[spiked][post] depends on timeSinceSpike[post]
    // if timeSinceSpike[post] small, w go down :(
    
    // TODO - integrate all inputs at the end of the step, to make bigger changes possible
    yA[post] += generalImpulse * connectionMatrix[spiked][post] * inhibitory[spiked];
    if (yA[post] > (generalThreshold*excitability[post])) {
        queueNeuron(post, 1); // insert in next step
        // colorNeuron(post);
        float delta = LR/exp(timeSinceSpike[post]/TEMP);
        connectionMatrix[spiked][post] -= delta;
        totalSum -= delta;
    } else {
        yA[post] = 0.0f;
        excitability[post] -= 0.01f;
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
pair<float, float> whereIs(short id, float time) {
    if (time == -1.0f) {time = timeSinceSpike[id];}
    float decay = exp(-alpha * time);
    float theta = atan(omega/(1 - alpha));

    float co = cos(time * theta);
    float si = sin(time * theta);
    float x = decay * (xA[id] * co - yA[id] * si);
    float y = decay * (yA[id] * co + xA[id] * si);
    return make_pair(x, y);
}

// returns the next time the neuron will spike, and its position
int whenSpike(short id) {
    // cout << xA[id] << " " << yA[id] << endl;
    // ignore if point is close to basin
    if (xA[id]*xA[id] + yA[id]*yA[id] < generalThreshold) {
        return 0;
    }
    if (yA[id] == 0.0f) {
        return 0;
    }
    float nextSpike = (1.0f/omega) * atan(xA[id]/yA[id]);
    // cout << "Next " << nextSpike << endl;
    // If the timing is negative we take the next
    if (nextSpike <= 0){
        nextSpike += M_PI / omega;
        // cout << "PI " << M_PI/omega << endl;
    }
    // given the time, calculate position

    auto [first, second] = whereIs(id, nextSpike);    
    xA[id] = first;
    yA[id] = second;

    return static_cast<int>(std::ceil(nextSpike));
}