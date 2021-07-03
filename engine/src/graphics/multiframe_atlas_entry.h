#pragma once

#include <stdint.h>
#include "../math/ivec2.h"
#include "../math/vec4.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t firstFrame;
    uint16_t frameWidth;
    uint16_t frameHeight;
    uint16_t framesPerRow;
    uint16_t frameCount;
} MultiframeAtlasEntry;

Vec4 *MultiframeAtlasEntry_generateTexCoords(
        const MultiframeAtlasEntry *atlasEntry, uint16_t *outCount, IVec2 atlasSize);