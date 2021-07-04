#pragma once

#include <stdint.h>
#include "../math/ivec2.h"
#include "../math/vec4.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t frameCount;
    uint8_t firstFrame;
    uint8_t frameWidth;
    uint8_t frameHeight;
    uint8_t framesPerRow;
} MultiframeAtlasEntry;

Vec4 *MultiframeAtlasEntry_generateTexCoords(MultiframeAtlasEntry this, uint16_t *outCount, IVec2 atlasSize);