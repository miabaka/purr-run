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

void TilemapRenderer_init(TilemapRenderer *this, const char *atlasImagePath, const MultiframeAtlasEntry *atlasEntry) {
    IVec2 atlasSize;
    this->texAtlas = loadTexture(atlasImagePath, &atlasSize);
    this->frameTexCoords = MultiframeAtlasEntry_generateTexCoords(*atlasEntry, &this->frameCount, atlasSize);
}

void TilemapRenderer_destroy(TilemapRenderer *this) {
    free(this->frameTexCoords);
    glDeleteTextures(1, &this->texAtlas);
}

static inline void drawTile(uint8_t col, uint8_t row, Vec4 texCoords) {
    glTexCoord2f(texCoords.x, texCoords.y);
    glVertex2i(col, row);

    glTexCoord2f(texCoords.x, texCoords.w);
    glVertex2i(col, row + 1);

    glTexCoord2f(texCoords.z, texCoords.w);
    glVertex2i(col + 1, row + 1);

    glTexCoord2f(texCoords.z, texCoords.y);
    glVertex2i(col + 1, row);
}

// TODO: replace with the blazing fast shader-based renderer
// TODO: take frame numbers from tile-to-frame lut
void TilemapRenderer_render(TilemapRenderer *this, const Tilemap *map) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->texAtlas);

    glBegin(GL_QUADS);

    uint8_t mapWidth = map->width;
    const Tile *tiles = map->tiles;

    // offset by first non-air tile
    Vec4 *texCoords = this->frameTexCoords - 1;

    for (uint16_t nTile = 0; nTile < map->tileCount; nTile++) {
        Tile tile = tiles[nTile];

        if (tile != Tile_Air)
            drawTile(nTile % mapWidth, nTile / mapWidth, texCoords[tile]);
    }

    glEnd(); // GL_QUADS

    glDisable(GL_TEXTURE_2D);
}