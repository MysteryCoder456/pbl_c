#include "asteroid.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

void asteroid_genvertices(Asteroid *asteroid, size_t vertexCount) {
    asteroid->vertices = calloc(vertexCount, sizeof(Vector2));
    asteroid->vertexCount = vertexCount;
    float angleUnit = 2 * PI / vertexCount;

    for (int i = 0; i < vertexCount; i++) {
        float angle = i * angleUnit;
        float mag = 45; // TODO: randomise

        Vector2 v = {.x = cosf(angle), .y = sinf(angle)};
        v = Vector2Scale(v, mag);

        asteroid->vertices[i] = v;
    }
}

void asteroid_freevertices(Asteroid *asteroid) { free(asteroid->vertices); }

void asteroid_update(Asteroid *asteroid, float deltaTime) {
    asteroid->position = Vector2Add(
        asteroid->position, Vector2Scale(asteroid->velocity, deltaTime));
}

void asteroid_draw(Asteroid *asteroid) {
    Vector2 verts[asteroid->vertexCount + 1];

    for (int i = 0; i < asteroid->vertexCount; i++)
        verts[i] = Vector2Add(asteroid->position, asteroid->vertices[i]);

    verts[asteroid->vertexCount] = verts[0];
    DrawLineStrip(verts, asteroid->vertexCount + 1, RAYWHITE);
}
