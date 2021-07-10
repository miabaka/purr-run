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

uint16_t MultiframeAtlasEntry_getFrameCount(const MultiframeAtlasEntry *this);

uint16_t MultiframeAtlasEntry_generateTexCoords(MultiframeAtlasEntry this, Vec4 *outTexCoords, IVec2 atlasSize);