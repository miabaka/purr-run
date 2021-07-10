#include "tilemap.h"

#include <stddef.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>

static const uint8_t DOCK_FRAME_MAPPING[] = {
        0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 4, 4,
        0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 4, 4,
        5, 5, 6, 6, 5, 5, 6, 6, 7, 7, 8, 8, 7, 7, 9, 9,
        5, 5, 6, 6, 5, 5, 6, 6, 10, 10, 11, 11, 10, 10,
        12, 12, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2,
        2, 4, 4, 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2,
        2, 4, 4, 5, 5, 6, 6, 5, 5, 6, 6, 7, 7, 8, 8, 7,
        7, 9, 9, 5, 5, 6, 6, 5, 5, 6, 6, 10, 10, 11, 11,
        10, 10, 12, 12, 13, 13, 14, 15, 13, 13, 14, 15,
        16, 16, 17, 18, 16, 16, 19, 20, 13, 13, 14, 15,
        13, 13, 14, 15, 16, 16, 17, 18, 16, 16, 19, 20,
        21, 21, 22, 23, 21, 21, 22, 23, 24, 24, 25, 26,
        24, 24, 27, 28, 21, 21, 22, 23, 21, 21, 22, 23,
        29, 29, 30, 31, 29, 29, 32, 33, 13, 13, 14, 15,
        13, 13, 14, 15, 16, 16, 17, 18, 16, 16, 19, 20,
        13, 13, 14, 15, 13, 13, 14, 15, 16, 16, 17, 18,
        16, 16, 19, 20, 34, 34, 35, 36, 34, 34, 35, 36,
        37, 37, 38, 39, 37, 37, 40, 41, 34, 34, 35, 36,
        34, 34, 35, 36, 42, 42, 43, 44, 42, 42, 45, 46
};

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

static inline bool tileIsDockable(TileType tile) {
    return tile == Tile_Ground || tile == Tile_Concrete || tile == Tile_Ice;
}

static inline uint8_t getDockingFrame(const Tilemap *map, IVec2 position) {
    uint8_t sides = 0;

    sides |= tileIsDockable(Tilemap_getTileWithOffset(map, position, -1, -1).type);
    sides |= tileIsDockable(Tilemap_getTileWithOffset(map, position, 0, -1).type) << 1;
    sides |= tileIsDockable(Tilemap_getTileWithOffset(map, position, 1, -1).type) << 2;
    sides |= tileIsDockable(Tilemap_getTileWithOffset(map, position, 1, 0).type) << 3;
    sides |= tileIsDockable(Tilemap_getTileWithOffset(map, position, 1, 1).type) << 4;
    sides |= tileIsDockable(Tilemap_getTileWithOffset(map, position, 0, 1).type) << 5;
    sides |= tileIsDockable(Tilemap_getTileWithOffset(map, position, -1, 1).type) << 6;
    sides |= tileIsDockable(Tilemap_getTileWithOffset(map, position, -1, 0).type) << 7;

    return DOCK_FRAME_MAPPING[sides];
}

Tile Tilemap_getTile(const Tilemap *this, IVec2 position) {
    if (position.x < 0 || position.y < 0 || position.x >= this->width || position.y >= this->height) {
        Tile airTile = {
                .type = Tile_Air,
                .variant = 0
        };

        return airTile;
    }

    return this->tiles[position.x + position.y * this->width];
}

void Tilemap_assignTiles(Tilemap *this, uint8_t width, uint8_t height, const TileType *tiles) {
    Tilemap_freeTiles(this);

    uint16_t tileCount = width * height;

    this->width = width;
    this->height = height;
    this->tileCount = tileCount;
    this->tiles = malloc(sizeof(Tile) * tileCount);

    for (uint16_t nTile = 0; nTile < tileCount; nTile++)
        this->tiles[nTile].type = tiles[nTile];

    IVec2 pos;

    for (pos.y = 0; pos.y < this->height; pos.y++) {
        for (pos.x = 0; pos.x < this->width; pos.x++) {
            Tile *tile = &this->tiles[pos.x + pos.y * this->width];

            if (tileIsDockable(tile->type))
                tile->variant = getDockingFrame(this, pos);
            else
                tile->variant = 0;
        }
    }
}

Tile Tilemap_getTileWithOffset(const Tilemap *this, IVec2 position, int ox, int oy) {
    position.x += ox;
    position.y += oy;

    return Tilemap_getTile(this, position);
}

void Tilemap_setTile(Tilemap *this, IVec2 position, TileType tileType) {
    if (position.x < 0 || position.y < 0 || position.x >= this->width || position.y >= this->height)
        return;

    Tile *tile = &this->tiles[position.x + position.y * this->width];

    tile->type = tileType;

    if (tileIsDockable(tileType))
        tile->variant = getDockingFrame(this, position);
    else
        tile->variant = 0;
}