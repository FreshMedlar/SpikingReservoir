#include "simulation.h"

Simulation::Simulation(short size, 
                        float LR, 
                        vector<short> encodedTraining, 
                        bool draw, 
                        bool train)
    :   SIZE(size), 
        manager(*new Manager(size, *this)), 
        scheduler(*new Scheduler(size, *this)),
        network(*new SingleLayerNetwork(LR, size)),
        encodedTraining(encodedTraining),
        train(train),
        draw(draw),
        currentSpikeIndex(0),
        currentColorIndex(0)
{
    connectionMatrix.resize(SIZE, vector<float>(SIZE, 0.0f));
    receivers.resize(SIZE);
    senders.resize(SIZE);
    
    biases.resize(SIZE);
    colors.resize(SIZE, WHITE);
    timeSinceSpike.resize(SIZE, 1000);
    active.resize(SIZE, true);
    inhibitory.resize(SIZE);
    xCoord.resize(SIZE);
    yCoord.resize(SIZE);
    xA.resize(SIZE, 0.0f);
    yA.resize(SIZE, 0.0f);
    excitability.resize(SIZE, 1.0f);
    frequency.resize(SIZE, 0.0f);
    threshold.resize(SIZE, 30);

    manager.createNeurons();
    manager.initialConnections();
    // OR
    // manager.createSequentialNeurons();

    manager.removeInputConnections(65);
    // spikeNumber.reserve(200);
} 

float Simulation::simulate() {
    if (draw) {
        InitWindow(1920, 1080, "Raylib - Circle Manager");
        ToggleFullscreen();    
        SetTargetFPS(-1);
    }
    float loss;
    float epoch_loss;

    for (int letter = 0; letter < encodedTraining.size()-1; letter++) {
        for (int cycle = 0; cycle < CYCLE_LEN; cycle++) {
            // spikeNumber[letter%200] = spikeBuffer[currentSpikeIndex].size();
            scheduler.step(encodedTraining[letter]);

            if (cycle%CYCLE_LEN == CYCLE_LEN-1 && train) {
                short target = encodedTraining[letter+1];
                Eigen::VectorXf output = network.forward_sparse(spikeBuffer[currentSpikeIndex]);
                output = network.softmax(output);
                loss = network.compute_loss(output, target);
                epoch_loss += loss;
                Eigen::VectorXf d_input = network.backward(spikeBuffer[currentSpikeIndex], output, target); // 10000, 

                if (letter%1000 == 0) {
                    cout << "Epoch " << letter+1
                        << " | Avg Loss: " << epoch_loss/1000
                        << endl;
                    epoch_loss = 0;
                } 
            }

            if (draw) {
                BeginDrawing();
                ClearBackground(BLACK);
                
                //DRAW
                // manager.draw();
                // manager.applyForces();
                //GRAPH
                connectionsPerNeuron.clear();
                connectionsPerNeuron.resize(SIZE, 0); 
                // EITHER, NOT BOTH
                manager.receiversFrequence(connectionsPerNeuron.data()); 
                // manager.sendersFrequence(connectionsPerNeuron.data());
                manager.drawreceiversGraph(connectionsPerNeuron); // Draw the plot
                // manager.clustering();
                // SPIKES
                manager.drawSpikesGraph(spikeNumber);
                // manager.drawTotalWeight();
                manager.drawOrder();
                manager.drawChaos();
                std::vector<float> sorted = frequency;  // Copy the original vector
                std::sort(sorted.begin(), sorted.end());  // Sort in ascending order
                manager.drawSpikeFrequencyDistribution(sorted);
    
                // cout << excitability[1000] << endl;
    
                // FPS  
                int fps = GetFPS();
                DrawText(TextFormat("FPS: %d", fps), 10, 10, 20, GREEN); 
    
                EndDrawing();
                
                scheduler.updateColor();
            }


        }
    }
    return loss;
}
