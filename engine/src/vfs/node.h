#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "node_type.h"

typedef struct VfsNode VfsNode;

struct VfsNode {
    char *name;
    VfsNode *child;
    VfsNode *next;
    VfsNodeType type;
    uint64_t offset;
    uint32_t size;
    bool zlibCompressed;
    uint8_t zlibFlags;
};