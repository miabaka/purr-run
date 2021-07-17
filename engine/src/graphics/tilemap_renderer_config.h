#pragma once

#include "multiframe_atlas_entry.h"
#include "tile_def.h"

typedef struct {
    const MultiframeAtlasEntry *atlasEntries;
    const TileDef *tileDefs;
    uint16_t atlasEntryCount;
    uint16_t tileDefCount;
} TilemapRendererConfig;

const TileDef *TilemapRendererConfig_getTileDef(const TilemapRendererConfig *this, TileType tileType);

uint16_t TilemapRendererConfig_getTileFrameCount(const TilemapRendererConfig *this, TileType tileType);