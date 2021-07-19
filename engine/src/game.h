#pragma once

#include <stdbool.h>
#include "math/vec2.h"
#include "math/ivec2.h"
#include "tilemap.h"
#include "graphics/tilemap_renderer.h"
#include "player.h"
#include "deferred_tile_update_list.h"
#include "treasure_list.h"
#include "system/window.h"
#include "vfs/package.h"

typedef struct {
    Player player;
    Tilemap map;
    TilemapRenderer mapRenderer;
    DeferredTileUpdateList tileUpdates;
    TreasureList treasures;
    uint16_t treasureFrameCount;
} Game;

void Game_init(Game *this, VfsPackage *vfsPackage);

void Game_destroy(Game *this);

void Game_update(Game *this, Window *window, float dt);

void Game_render(Game *this);

void Game_resize(Game *this, IVec2 size);