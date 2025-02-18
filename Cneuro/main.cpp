#include <iostream>
#include "neuron.h"
#include "manager.h"
#include "raylib.h"
#include "vector"
#include "scheduler.h"
#include "global.h"

const int SIZE = 1000;

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
    SetTargetFPS(60);
    int frameCounter = 0;
    // for (int i = 0; i < 1500; i++) {manager.applyForces();};

    std::uniform_real_distribution<> dis(0.0,1.0);
    std::uniform_real_distribution<> disreal(0, SIZE);
    while (!WindowShouldClose()) {
        frameCounter++;
        scheduler.changeColor();
        BeginDrawing();
        ClearBackground(BLACK); // Black background
        scheduler.update();
        manager.draw();
        // manager.applyForces();
        
        if (dis(gen) > 0.5) {
            neurons[disreal(gen)].spike(nullptr);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
