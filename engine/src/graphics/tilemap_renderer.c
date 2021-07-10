#include "tilemap_renderer.h"

#include <GL/glext.h>
#include "../stb/stb_image.h"

static GLuint loadTexture(const char *path, IVec2 *outSize) {
    int width, height, nChannels;
    unsigned char *pixelData = stbi_load(path, &width, &height, &nChannels, STBI_rgb_alpha);

    if (pixelData == NULL) {
        outSize->x = 0;
        outSize->y = 0;
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

    stbi_image_free(pixelData);

    outSize->x = width;
    outSize->y = height;

    return texture;
}

void TilemapRenderer_init(TilemapRenderer *this, const char *atlasImagePath, const TilemapRendererConfig *config) {
    IVec2 atlasSize;
    this->texAtlas = loadTexture(atlasImagePath, &atlasSize);

    uint16_t totalFrames = 0;

    for (uint16_t nEntry = 0; nEntry < config->atlasEntryCount; nEntry++)
        totalFrames += MultiframeAtlasEntry_getFrameCount(&config->atlasEntries[nEntry]);

    Vec4 *texCoords = malloc(sizeof(Vec4) * totalFrames);
    uint16_t *entryFirstFrames = malloc(config->atlasEntryCount);

    uint16_t processedFrameCount = 0;

    for (uint16_t nEntry = 0; nEntry < config->atlasEntryCount; nEntry++) {
        entryFirstFrames[nEntry] = processedFrameCount;

        processedFrameCount += MultiframeAtlasEntry_generateTexCoords(
                config->atlasEntries[nEntry], texCoords + processedFrameCount, atlasSize);
    }

    this->texCoords = texCoords;

    for (uint16_t nTileDef = 0; nTileDef < config->tileDefCount; nTileDef++) {
        const TileDef *tileDef = &config->tileDefs[nTileDef];

        if (tileDef->tile >= sizeof(this->tileFirstFrames) || tileDef->atlasEntry >= config->atlasEntryCount)
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
// TODO: take frame numbers from tile-to-frame lut
void TilemapRenderer_render(TilemapRenderer *this, const Tilemap *map) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->texAtlas);

    glBegin(GL_QUADS);

    uint8_t mapWidth = map->width;
    const Tile *tiles = map->tiles;

    for (uint16_t nTile = 0; nTile < map->tileCount; nTile++) {
        Tile tile = tiles[nTile];

        if (tile.type == Tile_Air)
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