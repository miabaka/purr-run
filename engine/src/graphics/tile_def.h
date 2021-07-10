#pragma once

#include <stdbool.h>
#include "../tile.h"

typedef struct {
    Tile tile;
    uint16_t atlasEntry;
    uint16_t firstFrame;
} TileDef;