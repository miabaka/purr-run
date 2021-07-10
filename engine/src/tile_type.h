#pragma once

typedef enum {
    TileType_Air = 0,
    TileType_Ground = 1,
    TileType_Concrete = 2,
    TileType_Ice = 3,
    TileType_Ladder = 4,
    TileType_Rope = 5,
    TileType_count
} __attribute__((packed)) TileType;