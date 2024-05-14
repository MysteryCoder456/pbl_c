#include "asteroid.h"
#include "ship.h"
#include <raylib.h>

#define WIDTH 1024.0
#define HEIGHT 720.0
#define PLAYER_ACCEL 400.0f
#define PLAYER_TURN_SPEED 4.0f

int main() {
    InitWindow(WIDTH, HEIGHT, "Asteroids");
    SetTargetFPS(60);

    Color bgColor = {25, 25, 25, 255};
    Ship player = {.position = {.x = WIDTH / 2, .y = HEIGHT / 2},
                   .velocity = {0, 0},
                   .angle = 0};

    Asteroid a;
    a.position = (Vector2){.x = WIDTH / 4, .y = HEIGHT / 4};
    a.velocity = (Vector2){.x = 15.0, .y = 10.0};
    asteroid_genvertices(&a, 8);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // ————— Update —————

        if (IsKeyDown(KEY_W))
            ship_accelerate(&player, PLAYER_ACCEL * deltaTime);
        if (IsKeyDown(KEY_A))
            player.angle -= 3.0f * deltaTime;
        if (IsKeyDown(KEY_D))
            player.angle += 3.0f * deltaTime;

        ship_update(&player, deltaTime);
        asteroid_update(&a, deltaTime);

        // ————— Draw —————

        BeginDrawing();
        ClearBackground(bgColor);

        ship_draw(&player);
        asteroid_draw(&a);

        EndDrawing();
    }

    // Free asteroid vertices
    asteroid_freevertices(&a);

    CloseWindow();
    return 0;
}
