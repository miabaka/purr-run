#pragma once

#include <stdint.h>
#include "tile.h"

typedef struct {
    Tile *tiles;
    uint16_t tileCount;
    uint8_t width;
    uint8_t height;
} Tilemap;

void Tilemap_init(Tilemap *this);

void Tilemap_destroy(Tilemap *this);

void Tilemap_freeTiles(Tilemap *this);

void Tilemap_assignTiles(Tilemap *this, uint8_t width, uint8_t height, const Tile *tiles);