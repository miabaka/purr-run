#include "multiframe_atlas_entry.h"

#include <malloc.h>
#include "../math/vec2.h"

uint16_t MultiframeAtlasEntry_getFrameCount(const MultiframeAtlasEntry *this) {
    return this->frameCount - this->firstFrame;
}

uint16_t MultiframeAtlasEntry_generateTexCoords(MultiframeAtlasEntry this, Vec4 *outTexCoords, IVec2 atlasSize) {
    Vec2 coordFactor = {
            .x = 1.f / (float) atlasSize.x,
            .y = 1.f / (float) atlasSize.y
    };

    Vec4 *currentTexCoords = outTexCoords;

    for (uint16_t nFrame = this.firstFrame; nFrame < this.frameCount; nFrame++) {
        uint16_t x = this.x + (nFrame % this.framesPerRow) * this.frameWidth;
        uint16_t y = this.y + (nFrame / this.framesPerRow) * this.frameHeight;

        currentTexCoords->x = (float) x * coordFactor.x;
        currentTexCoords->y = (float) y * coordFactor.y;
        currentTexCoords->z = (float) (x + this.frameWidth) * coordFactor.x;
        currentTexCoords->w = (float) (y + this.frameHeight) * coordFactor.y;

        currentTexCoords++;
    }

    return MultiframeAtlasEntry_getFrameCount(&this);
}