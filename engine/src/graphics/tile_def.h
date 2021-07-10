#pragma once

#include <stdbool.h>
#include "../tile_type.h"

typedef struct {
    TileType tile;
    uint16_t atlasEntry;
    uint16_t firstFrame;
} TileDef;