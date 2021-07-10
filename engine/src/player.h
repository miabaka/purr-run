#pragma once

#include "math/vec2.h"
#include "math/ivec2.h"
#include "player_state.h"
#include "player_input_state.h"
#include "tilemap.h"

#define PLAYER_MOVEMENT_SPEED 4.f
#define PLAYER_FALLING_SPEED 7.f
#define PLAYER_SNAPPING_RATE 60.f

typedef struct {
    Vec2 position;
    IVec2 tilePosition;
    int ropeFallRow;
    PlayerState state;
} Player;

void Player_init(Player *this);

void Player_destroy(Player *this);

void Player_update(Player *this, float dt, const PlayerInputState *inputState, Tilemap *map);

void Player_setPosition(Player *this, float x, float y);

void Player_render(Player *this);