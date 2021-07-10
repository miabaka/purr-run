#pragma once

#include <stdint.h>
#include "math/ivec2.h"
#include "tile_type.h"
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

Tile Tilemap_getTile(const Tilemap *this, IVec2 position);

Tile Tilemap_getTileWithOffset(const Tilemap *this, IVec2 position, int ox, int oy);

void Tilemap_setTile(Tilemap *this, IVec2 position, TileType tileType);

void Tilemap_assignTiles(Tilemap *this, uint8_t width, uint8_t height, const TileType *tiles);