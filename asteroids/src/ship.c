#include "ship.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>

void ship_update(Ship *ship, float deltaTime) {
    // Move ship
    ship->position =
        Vector2Add(ship->position, Vector2Scale(ship->velocity, deltaTime));

    // Velocity damping
    ship->velocity = Vector2Scale(ship->velocity, 0.98);
}

void ship_accelerate(Ship *ship, float acceleration) {
    Vector2 accelDirection = {.x = cosf(ship->angle), .y = sinf(ship->angle)};
    ship->velocity =
        Vector2Add(ship->velocity, Vector2Scale(accelDirection, acceleration));
}

void ship_draw(Ship *ship) {
    Vector2 v1 = {.x = cosf(ship->angle), .y = sinf(ship->angle)};
    v1 = Vector2Scale(v1, 27);
    v1 = Vector2Add(v1, ship->position);

    Vector2 v2 = {.x = cosf(ship->angle + 120 * DEG2RAD),
                  .y = sinf(ship->angle + 120 * DEG2RAD)};
    v2 = Vector2Scale(v2, 17);
    v2 = Vector2Add(v2, ship->position);

    Vector2 v3 = {.x = cosf(ship->angle - 120 * DEG2RAD),
                  .y = sinf(ship->angle - 120 * DEG2RAD)};
    v3 = Vector2Scale(v3, 17);
    v3 = Vector2Add(v3, ship->position);

    DrawTriangleLines(v1, v2, v3, RAYWHITE);
}
