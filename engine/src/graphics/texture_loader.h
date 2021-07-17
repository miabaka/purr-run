#pragma once

#include "../vfs/package.h"
#include "gl/texture.h"

GlTexture GlTextureLoader_loadFromVfs(VfsPackage *package, const char *path);