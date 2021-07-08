#pragma once

typedef struct {
    float x, y;
} Vec2;

void Vec2_add(Vec2 *this, Vec2 *other);

void Vec2_sub(Vec2 *this, Vec2 *other);

void Vec2_floor(Vec2 *this);