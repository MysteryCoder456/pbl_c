#ifndef _SHIP_H_
#define _SHIP_H_

#include <raylib.h>

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float angle;
} Ship;

void ship_draw(Ship *ship);

#endif
