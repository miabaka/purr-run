#pragma once

#include <stdint.h>
#include <GL/gl.h>
#include "multiframe_atlas_entry.h"
#include "../tilemap.h"
#include "../math/vec4.h"
#include "tilemap_renderer_config.h"

typedef struct {
    uint16_t tileFirstFrames[TileType_count];
    Vec4 *texCoords;
    GLuint texAtlas;
} TilemapRenderer;

void TilemapRenderer_init(TilemapRenderer *this, const char *atlasImagePath, const TilemapRendererConfig *config);

void TilemapRenderer_destroy(TilemapRenderer *this);

void TilemapRenderer_render(TilemapRenderer *this, const Tilemap *map);