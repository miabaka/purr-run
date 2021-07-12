#pragma once

#include "math/ivec2.h"
#include "tile_type.h"

typedef struct {
    IVec2 position;
    float timeLeft;
    TileType tileType;
} DeferredTileUpdate;