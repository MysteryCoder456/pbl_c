#include "ship.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>

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
