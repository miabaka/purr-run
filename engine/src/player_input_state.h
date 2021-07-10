#pragma once

#include <stdbool.h>

typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
    bool digLeft;
    bool digRight;
} PlayerInputState;