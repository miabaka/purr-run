#include "vec2.h"

#include <math.h>

void Vec2_add(Vec2 *this, Vec2 *other) {
    this->x += other->x;
    this->y += other->y;
}

void Vec2_sub(Vec2 *this, Vec2 *other) {
    this->x -= other->x;
    this->y -= other->y;
}

void Vec2_floor(Vec2 *this) {
    this->x = floorf(this->x);
    this->y = floorf(this->y);
}