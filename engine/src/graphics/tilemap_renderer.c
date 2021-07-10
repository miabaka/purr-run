#include "tilemap_renderer.h"

#include <GL/glext.h>
#include "../stb/stb_image.h"

static const uint8_t DOCK_MAPPING[] = {
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

static inline bool isTileDockable(Tile tile) {
    return tile == Tile_Ground || tile == Tile_Concrete || tile == Tile_Ice;
}

static inline uint8_t getDockingFrame(const Tilemap *map, IVec2 position) {
    uint8_t sides = 0;

    sides |= isTileDockable(Tilemap_getTileWithOffset(map, position, -1, -1));
    sides |= isTileDockable(Tilemap_getTileWithOffset(map, position, 0, -1)) << 1;
    sides |= isTileDockable(Tilemap_getTileWithOffset(map, position, 1, -1)) << 2;
    sides |= isTileDockable(Tilemap_getTileWithOffset(map, position, 1, 0)) << 3;
    sides |= isTileDockable(Tilemap_getTileWithOffset(map, position, 1, 1)) << 4;
    sides |= isTileDockable(Tilemap_getTileWithOffset(map, position, 0, 1)) << 5;
    sides |= isTileDockable(Tilemap_getTileWithOffset(map, position, -1, 1)) << 6;
    sides |= isTileDockable(Tilemap_getTileWithOffset(map, position, -1, 0)) << 7;

    return DOCK_MAPPING[sides];
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

        if (tile == Tile_Air)
            continue;

        uint16_t frame = this->tileFirstFrames[tile];

        IVec2 pos = {
                .x = nTile % mapWidth,
                .y = nTile / mapWidth
        };

        if (isTileDockable(tile))
            frame += getDockingFrame(map, pos);

        drawTile(pos, this->texCoords[frame]);
    }

    glEnd(); // GL_QUADS

    glDisable(GL_TEXTURE_2D);
}