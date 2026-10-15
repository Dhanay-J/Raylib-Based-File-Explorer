#include "raylib.h"

int main()
{
    InitWindow(800, 450, "Raylib Rendering Test");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Raylib rendering works!", 100, 100, 30, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}