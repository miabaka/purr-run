#include "multiframe_atlas_entry.h"

#include <malloc.h>
#include "../math/vec2.h"

Vec4 *MultiframeAtlasEntry_generateTexCoords(MultiframeAtlasEntry this, uint16_t *outCount, IVec2 atlasSize) {
    uint16_t frameCount = this.frameCount - this.firstFrame;

    Vec2 coordFactor = {
            .x = 1.f / (float) atlasSize.x,
            .y = 1.f / (float) atlasSize.y
    };

    Vec4 *texCoords = malloc(sizeof(Vec4) * frameCount);
    Vec4 *currentTexCoords = texCoords;

    for (uint16_t nFrame = this.firstFrame; nFrame < this.frameCount; nFrame++) {
        uint16_t x = (nFrame % this.framesPerRow) * this.frameWidth;
        uint16_t y = (nFrame / this.framesPerRow) * this.frameHeight;

        currentTexCoords->x = (float) x * coordFactor.x;
        currentTexCoords->y = (float) y * coordFactor.y;
        currentTexCoords->z = (float) (x + this.frameWidth) * coordFactor.x;
        currentTexCoords->w = (float) (y + this.frameHeight) * coordFactor.y;

        currentTexCoords++;
    }

    *outCount = frameCount;

    return texCoords;
}