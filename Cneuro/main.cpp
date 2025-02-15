#include <iostream>
#include "neuron.h"
#include "manager.h"
#include "raylib.h"
#include "vector"
#include "scheduler.h"

extern std::vector<Neuron> neurons;

int main() {
    Manager manager;
    Scheduler scheduler;
    manager.createNeurons(10, &scheduler);
    manager.initialConnections();
    manager.status();

    const int screenWidth = 1600;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Raylib - Circle Manager");
    ToggleFullscreen();    
    SetTargetFPS(60);
    // for (int i = 0; i < 1000; i++) {manager.applyForces();};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK); // Black background
        manager.draw();
        manager.applyForces();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
