#ifndef _ASTEROID_H
#define _ASTEROID_H

#include <raylib.h>
#include <stdlib.h>

typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 *vertices;
    size_t vertexCount;
} Asteroid;

void asteroid_genvertices(Asteroid *asteroid, size_t vertexCount);
void asteroid_freevertices(Asteroid *asteroid);
void asteroid_update(Asteroid *asteroid, float deltaTime);
void asteroid_draw(Asteroid *asteroid);

#endif
