#include "package.h"

#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include "../stb/stb_image.h"

#define VFS_PATH_DELIMITER "/"

typedef struct {
    uint8_t formatVersion: 7;
    uint8_t nodeInfoCompressed: 1;
    uint16_t nodeCount;
    uint16_t rootNode;
} __attribute__((packed)) RawCommonHeader;

typedef struct {
    uint8_t type: 1;
    uint8_t hasChild: 1;
    uint8_t hasNext: 1;
    uint8_t zlibCompressed: 1;
    uint8_t zlibFlags: 3;
    uint8_t nameLength;
} __attribute__((packed)) RawNode;

static void readNode(FILE *fp, VfsNode *firstNode, VfsNode *node) {
    RawNode rawNode;
    fread(&rawNode, sizeof(RawNode), 1, fp);

    node->type = rawNode.type;
    node->zlibCompressed = rawNode.zlibCompressed;
    node->zlibFlags = rawNode.zlibFlags;

    {
        char *name;

        if (rawNode.nameLength > 0) {
            name = malloc(rawNode.nameLength + 1);
            name[rawNode.nameLength] = '\0';
            fread(name, sizeof(char), rawNode.nameLength, fp);
        } else {
            name = NULL;
        }

        node->name = name;
    }

    if (rawNode.hasChild) {
        uint16_t childNodeId;
        fread(&childNodeId, sizeof(childNodeId), 1, fp);

        node->child = firstNode + childNodeId;
    } else {
        node->child = NULL;
    }

    if (rawNode.hasNext) {
        uint16_t nextNodeId;
        fread(&nextNodeId, sizeof(nextNodeId), 1, fp);

        node->next = firstNode + nextNodeId;
    } else {
        node->next = NULL;
    }

    if (rawNode.type == VfsNodeType_File)
        fread(&node->size, sizeof(node->size), 1, fp);
    else
        node->size = 0;
}

// TODO: proper error checking
VfsResult VfsPackage_init(VfsPackage *this, const char *path) {
    this->fp = NULL;
    this->nodes = NULL;

    FILE *fp = fopen(path, "rb");

    if (!fp)
        return VfsResult_FsError;

    char magic[4];
    fread(&magic, sizeof(magic), 1, fp);

    if (memcmp(magic, "PRPK", sizeof(magic)) != 0) {
        fclose(fp);
        return VfsResult_WrongMagic;
    }

    RawCommonHeader commonHeader;
    fread(&commonHeader, sizeof(commonHeader), 1, fp);

    if (commonHeader.formatVersion != 0 || commonHeader.nodeInfoCompressed) {
        fclose(fp);
        return VfsResult_UnsupportedFormat;
    }

    VfsNode *nodes = malloc(sizeof(VfsNode) * commonHeader.nodeCount);

    for (uint16_t nNode = 0; nNode < commonHeader.nodeCount; nNode++)
        readNode(fp, nodes, &nodes[nNode]);

    {
        uint64_t offset = ftell(fp);

        for (uint16_t nNode = 0; nNode < commonHeader.nodeCount; nNode++) {
            VfsNode *node = &nodes[nNode];

            if (node->type == VfsNodeType_File) {
                node->offset = offset;
                offset += node->size;
            } else {
                node->offset = 0;
            }
        }
    }

    this->fp = fp;
    this->nodes = nodes;
    this->nodeCount = commonHeader.nodeCount;
    this->rootNode = &nodes[commonHeader.rootNode];

    return VfsResult_Success;
}

void VfsPackage_destroy(VfsPackage *this) {
    if (this->nodes)
        free(this->nodes);

    if (this->fp)
        fclose(this->fp);
}

// TODO: proper path parsing including . and ..
VfsNode *VfsPackage_findNode(VfsPackage *this, const char *path) {
    VfsNode *currentNode = this->rootNode;

    if (!currentNode->child)
        return NULL;

    char *writablePath = strdup(path);
    char *tokContext;

    const char *token = strtok_s(writablePath, VFS_PATH_DELIMITER, &tokContext);

    while (token) {
        currentNode = currentNode->child;

        while (currentNode) {
            if (strcmp(token, currentNode->name) == 0)
                break;

            currentNode = currentNode->next;
        }

        token = strtok_s(NULL, VFS_PATH_DELIMITER, &tokContext);

        if (!(currentNode && currentNode->child))
            break;
    }

    free(writablePath);

    return currentNode;
}

void *VfsPackage_readFile(VfsPackage *this, const char *path, uint32_t *outSize) {
    VfsNode *node = VfsPackage_findNode(this, path);

    if (!(node && node->type == VfsNodeType_File))
        return NULL;

    fseek(this->fp, (long) node->offset, SEEK_SET);

    if (node->zlibCompressed) {
        int bufSize = (int) (node->size + 2);
        void *buf = malloc(bufSize);

        uint8_t flags = (node->zlibFlags & 7) << 5;

        // calculate check bits (fcheck)
        flags += 31 - ((0x78 * 256 + flags) % 31);

        ((uint8_t *) buf)[0] = 0x78;
        ((uint8_t *) buf)[1] = flags;

        fread(buf + 2, node->size, 1, this->fp);

        int decodedBufSize;
        void *decodedBuf = stbi_zlib_decode_malloc(buf, bufSize, &decodedBufSize);

        free(buf);

        *outSize = decodedBufSize;

        return decodedBuf;
    }

    void *buf = malloc(node->size);
    fseek(this->fp, (long) node->offset, SEEK_SET);

    *outSize = node->size;

    return buf;
}