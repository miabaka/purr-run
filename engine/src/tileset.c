#include "graphics/multiframe_atlas_entry.h"
#include "graphics/tile_def.h"
#include "graphics/tilemap_renderer_config.h"

static const MultiframeAtlasEntry TILESET_ATLAS_ENTRIES[] = {
        {
                .x = 0,
                .y = 0,
                .frameCount = 47,
                .firstFrame = 0,
                .framesPerRow = 16,
                .frameWidth = 32,
                .frameHeight = 32
        },
        {
                .x = 480,
                .y = 64,
                .frameCount = 1,
                .firstFrame = 0,
                .framesPerRow = 1,
                .frameWidth = 32,
                .frameHeight = 32
        },
        {
                .x = 0,
                .y = 128,
                .frameCount = 18,
                .firstFrame = 0,
                .framesPerRow = 16,
                .frameWidth = 32,
                .frameHeight = 32
        },
        {
                .x = 0,
                .y = 96,
                .frameCount = 4,
                .firstFrame = 0,
                .framesPerRow = 4,
                .frameWidth = 32,
                .frameHeight = 32
        }
};

static const TileDef TILESET_TILE_DEFS[] = {
        {
                .tile = TileType_Ground,
                .atlasEntry = 0,
                .firstFrame = 0
        },
        {
                .tile = TileType_Concrete,
                .atlasEntry = 0,
                .firstFrame = 0
        },
        {
                .tile = TileType_Ice,
                .atlasEntry = 0,
                .firstFrame = 0
        },
        {
                .tile = TileType_Ladder,
                .atlasEntry = 3,
                .firstFrame = 0
        },
        {
                .tile = TileType_Rope,
                .atlasEntry = 1,
                .firstFrame = 0
        },
        {
                .tile = TileType_Treasure,
                .atlasEntry = 2,
                .firstFrame = 0
        }
};

static const TilemapRendererConfig TILESET_RENDERER_CONFIG = {
        .atlasEntries = TILESET_ATLAS_ENTRIES,
        .atlasEntryCount = sizeof(TILESET_ATLAS_ENTRIES) / sizeof(MultiframeAtlasEntry),
        .tileDefs = TILESET_TILE_DEFS,
        .tileDefCount = sizeof(TILESET_TILE_DEFS) / sizeof(TileDef)
};

const TilemapRendererConfig *Tileset_getRendererConfig() {
    return &TILESET_RENDERER_CONFIG;
}