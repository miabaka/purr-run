#pragma once

#include <stdint.h>
#include <stdio.h>
#include "node.h"
#include "result.h"

typedef struct {
    FILE *fp;
    VfsNode *nodes;
    VfsNode *rootNode;
    uint16_t nodeCount;
} VfsPackage;

VfsResult VfsPackage_init(VfsPackage *this, const char *path);

void VfsPackage_destroy(VfsPackage *this);

VfsNode *VfsPackage_findNode(VfsPackage *this, const char *path);

void *VfsPackage_readFile(VfsPackage *this, const char *path, uint32_t *outSize);