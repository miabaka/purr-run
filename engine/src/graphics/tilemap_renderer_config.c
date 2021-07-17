#include "tilemap_renderer_config.h"

#include <stddef.h>

const TileDef *TilemapRendererConfig_getTileDef(const TilemapRendererConfig *this, TileType tileType) {
    const TileDef *def = this->tileDefs;

    for (uint16_t nDef = 0; nDef < this->tileDefCount; nDef++) {
        if (def->tile == tileType)
            return def;

        def++;
    }

    return NULL;
}

uint16_t TilemapRendererConfig_getTileFrameCount(const TilemapRendererConfig *this, TileType tileType) {
    const TileDef *tileDef = TilemapRendererConfig_getTileDef(this, tileType);

    if (!tileDef)
        return 0;

    return MultiframeAtlasEntry_getFrameCount(&this->atlasEntries[tileDef->atlasEntry]);
}