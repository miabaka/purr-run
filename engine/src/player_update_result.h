#pragma once

#include "math/ivec2.h"

typedef enum {
    PlayerUpdateResultType_None,
    PlayerUpdateResultType_Dig
} PlayerUpdateResultType;

typedef struct {
    PlayerUpdateResultType type;
    IVec2 position;
} PlayerUpdateResult;