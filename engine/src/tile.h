#pragma once

#include <stdint.h>
#include "tile_type.h"

typedef struct {
    TileType type;
    uint8_t variant;
} Tile;