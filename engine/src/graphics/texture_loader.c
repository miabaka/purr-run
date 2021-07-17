#include "texture_loader.h"

#include <GL/glext.h>
#include "../stb/stb_image.h"

GlTexture GlTextureLoader_loadFromVfs(VfsPackage *package, const char *path) {
    GlTexture texture = {
            .handle = 0,
            .size = {-1, -1}
    };

    int nChannels;

    uint32_t dataSize;
    void *data = VfsPackage_readFile(package, path, &dataSize);

    if (!data)
        return texture;

    void *pixelData =
            stbi_load_from_memory(data, (int) dataSize, &texture.size.x, &texture.size.y, &nChannels, STBI_rgb_alpha);

    free(data);

    if (!pixelData)
        return texture;

    glGenTextures(1, &texture.handle);

    glBindTexture(GL_TEXTURE_2D, texture.handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.size.x, texture.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

    stbi_image_free(pixelData);

    return texture;
}