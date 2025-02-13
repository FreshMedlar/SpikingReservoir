#include <iostream>
#include "neuron.h"
#include "manager.h"
#include "raylib.h"
#include "vector"

int main() {
    Manager manager(50);
    manager.status();
    
    const int screenWidth = 1600;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Raylib - Circle Manager");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK); // Black background
        manager.draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
