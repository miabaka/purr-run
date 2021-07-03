#pragma once

#include <GL/gl.h>
#include "multiframe_atlas_entry.h"
#include "../tilemap.h"
#include "../math/vec4.h"

typedef struct {
    Vec4 *frameTexCoords;
    GLuint texAtlas;
    uint16_t frameCount;
} TilemapRenderer;

void TilemapRenderer_init(TilemapRenderer *this, const char *atlasImagePath, const MultiframeAtlasEntry *atlasEntry);

void TilemapRenderer_destroy(TilemapRenderer *this);

void TilemapRenderer_render(TilemapRenderer *this, const Tilemap *map);