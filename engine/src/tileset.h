#pragma once

#include <stddef.h>
#include "graphics/multiframe_atlas_entry.h"
#include "graphics/tile_def.h"
#include "graphics/tilemap_renderer_config.h"

const MultiframeAtlasEntry TILESET_ATLAS_ENTRIES[] = {
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
                .x = 0,
                .y = 64,
                .frameCount = 17,
                .firstFrame = 15,
                .framesPerRow = 16,
                .frameWidth = 32,
                .frameHeight = 32
        }
};

const TileDef TILESET_TILE_DEFS[] = {
        {
                .tile = Tile_Ground,
                .atlasEntry = 0,
                .firstFrame = 0
        },
        {
                .tile = Tile_Concrete,
                .atlasEntry = 0,
                .firstFrame = 0
        },
        {
                .tile = Tile_Ice,
                .atlasEntry = 0,
                .firstFrame = 0
        },
        {
                .tile = Tile_Ladder,
                .atlasEntry = 1,
                .firstFrame = 0
        },
        {
                .tile = Tile_Rope,
                .atlasEntry = 1,
                .firstFrame = 1
        }
};

const TilemapRendererConfig TILESET_RENDERER_CONFIG = {
        .atlasEntries = TILESET_ATLAS_ENTRIES,
        .atlasEntryCount = sizeof(TILESET_ATLAS_ENTRIES) / sizeof(MultiframeAtlasEntry),
        .tileDefs = TILESET_TILE_DEFS,
        .tileDefCount = sizeof(TILESET_TILE_DEFS) / sizeof(TileDef)
};