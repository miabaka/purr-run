#include "tilemap.h"

#include <stddef.h>
#include <malloc.h>
#include <string.h>

void Tilemap_init(Tilemap *this) {
    this->tiles = NULL;
    this->width = 0;
    this->height = 0;
}

void Tilemap_destroy(Tilemap *this) {
    Tilemap_freeTiles(this);
}

void Tilemap_freeTiles(Tilemap *this) {
    if (this->tiles != NULL)
        free(this->tiles);
}

void Tilemap_assignTiles(Tilemap *this, uint8_t width, uint8_t height, const Tile *tiles) {
    Tilemap_freeTiles(this);

    uint16_t tileCount = width * height;
    size_t sizeInBytes = sizeof(Tile) * tileCount;

    this->width = width;
    this->height = height;
    this->tileCount = tileCount;
    this->tiles = malloc(sizeInBytes);

    memcpy(this->tiles, tiles, sizeInBytes);
}

Tile Tilemap_getTile(Tilemap *this, IVec2 position) {
    if (position.x < 0 || position.y < 0 || position.x >= this->width || position.y >= this->height)
        return Tile_Air;

    return this->tiles[position.x + position.y * this->width];
}

Tile Tilemap_getTile2(Tilemap *this, IVec2 position, int ox, int oy) {
    position.x += ox;
    position.y += oy;

    return Tilemap_getTile(this, position);
}

void Tilemap_setTile(Tilemap *this, IVec2 position, Tile tile) {
    if (position.x < 0 || position.y < 0 || position.x >= this->width || position.y >= this->height)
        return;

    this->tiles[position.x + position.y * this->width] = tile;
}