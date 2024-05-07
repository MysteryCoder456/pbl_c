#include "ship.h"
#include <raylib.h>

#define WIDTH 1024.0
#define HEIGHT 720.0

int main() {
    InitWindow(WIDTH, HEIGHT, "Asteroids");
    SetTargetFPS(60);

    Color bgColor = {25, 25, 25, 255};

    Ship player = {.position = {.x = WIDTH / 2, .y = HEIGHT / 2},
                   .velocity = {0, 0},
                   .angle = 0};

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(bgColor);

        ship_draw(&player);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
