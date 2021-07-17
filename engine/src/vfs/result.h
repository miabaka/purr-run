#pragma once

typedef enum {
    VfsResult_Success,
    VfsResult_FsError,
    VfsResult_WrongMagic,
    VfsResult_UnsupportedFormat
} VfsResult;