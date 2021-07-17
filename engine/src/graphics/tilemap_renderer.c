#include "tilemap_renderer.h"

#include <GL/glext.h>
#include "../stb/stb_image.h"

void TilemapRenderer_init(TilemapRenderer *this, GlTexture texture, const TilemapRendererConfig *config) {
    this->texAtlas = texture.handle;

    uint16_t totalFrames = 0;

    for (uint16_t nEntry = 0; nEntry < config->atlasEntryCount; nEntry++)
        totalFrames += MultiframeAtlasEntry_getFrameCount(&config->atlasEntries[nEntry]);

    Vec4 *texCoords = malloc(sizeof(Vec4) * totalFrames);
    uint16_t *entryFirstFrames = malloc(config->atlasEntryCount);

    uint16_t processedFrameCount = 0;

    for (uint16_t nEntry = 0; nEntry < config->atlasEntryCount; nEntry++) {
        entryFirstFrames[nEntry] = processedFrameCount;

        processedFrameCount += MultiframeAtlasEntry_generateTexCoords(
                config->atlasEntries[nEntry], texCoords + processedFrameCount, texture.size);
    }

    this->texCoords = texCoords;

    for (uint16_t nTileDef = 0; nTileDef < config->tileDefCount; nTileDef++) {
        const TileDef *tileDef = &config->tileDefs[nTileDef];

        if (tileDef->tile >= TileType_count || tileDef->atlasEntry >= config->atlasEntryCount)
            continue;

        uint16_t firstFrame = entryFirstFrames[tileDef->atlasEntry] + tileDef->firstFrame;

        if (firstFrame >= processedFrameCount)
            continue;

        this->tileFirstFrames[tileDef->tile] = firstFrame;
    }

    free(entryFirstFrames);
}

void TilemapRenderer_destroy(TilemapRenderer *this) {
    free(this->texCoords);
    glDeleteTextures(1, &this->texAtlas);
}

static inline void drawTile(IVec2 position, Vec4 texCoords) {
    glTexCoord2f(texCoords.x, texCoords.y);
    glVertex2i(position.x, position.y);

    glTexCoord2f(texCoords.x, texCoords.w);
    glVertex2i(position.x, position.y + 1);

    glTexCoord2f(texCoords.z, texCoords.w);
    glVertex2i(position.x + 1, position.y + 1);

    glTexCoord2f(texCoords.z, texCoords.y);
    glVertex2i(position.x + 1, position.y);
}

// TODO: replace with the blazing fast shader-based renderer
void TilemapRenderer_render(TilemapRenderer *this, const Tilemap *map) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->texAtlas);

    glBegin(GL_QUADS);

    uint8_t mapWidth = map->width;
    const Tile *tiles = map->tiles;

    for (uint16_t nTile = 0; nTile < map->tileCount; nTile++) {
        Tile tile = tiles[nTile];

        if (tile.type == TileType_Air)
            continue;

        uint16_t frame = this->tileFirstFrames[tile.type];

        IVec2 pos = {
                .x = nTile % mapWidth,
                .y = nTile / mapWidth
        };

        drawTile(pos, this->texCoords[frame + tile.variant]);
    }

    glEnd(); // GL_QUADS

    glDisable(GL_TEXTURE_2D);
}