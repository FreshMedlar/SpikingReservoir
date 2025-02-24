#include <iostream>
#include "neuron.h"
#include "manager.h"
#include "raylib.h"
#include "vector"
#include "scheduler.h"
#include "global.h"
#include <algorithm>

int SIZE = 200;

int main() {
    Manager manager(SIZE);
    Scheduler scheduler(SIZE);
    manager.createNeurons(&scheduler);
    manager.initialConnections();
    manager.status();

    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Raylib - Circle Manager");
    ToggleFullscreen();    
    SetTargetFPS(0);
    int frameCounter = 0;
    std::vector<int> connectionsPerNeuron(SIZE, 0);
    // for (int i = 0; i < 1500; i++) {manager.applyForces();};

    std::uniform_real_distribution<> dis(0.0,1.0);
    std::uniform_real_distribution<> disreal(0, SIZE-1);
    while (!WindowShouldClose()) {
        // std::cout << "BEGIN" << std::endl;
        frameCounter++;
        BeginDrawing();
        ClearBackground(BLACK); // Black background

        // NEURONS DRAWING 
        scheduler.changeColor();
        manager.draw();
        // if (frameCounter%2 ==0) {manager.applyForces();}

        // GRAPHS DRAWING
        if (frameCounter%2 == 0) {
            connectionsPerNeuron.clear();
            connectionsPerNeuron.resize(SIZE, 0); 
            manager.countFrequence(connectionsPerNeuron.data()); 
        }
        manager.drawGraph(connectionsPerNeuron); // Draw the plot

        scheduler.update();
        scheduler.synaptoGenesis();
        if (dis(gen) > 0.8) {
            neurons[disreal(gen)].spike(nullptr);
        }

        // FPS
        int fps = GetFPS();
        DrawText(TextFormat("FPS: %d", fps), 10, 10, 20, GREEN); 
        
        EndDrawing();
        // std::cout << "END" << std::endl;
    }

    CloseWindow();

    return 0;
}