#pragma once

#include "multiframe_atlas_entry.h"
#include "tile_def.h"

typedef struct {
    const MultiframeAtlasEntry *atlasEntries;
    const TileDef *tileDefs;
    uint16_t atlasEntryCount;
    uint16_t tileDefCount;
} TilemapRendererConfig;