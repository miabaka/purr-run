#include "player.h"

#include <stdbool.h>
#include <math.h>
#include <GL/gl.h>
#include "math/util.h"

static inline bool isSolidTile(TileType tile) {
    return tile == TileType_Ground || tile == TileType_Concrete || tile == TileType_Ice;
}

static inline bool isFloorTile(TileType tile) {
    return isSolidTile(tile) || tile == TileType_Ladder;
}

static void snapAxisToGrid(float *pos, int tilePos, float dt) {
    *pos = lerpf((float) tilePos, *pos, exp2f(-PLAYER_SNAPPING_RATE * dt));
}

static inline void snapToHorizontalGrid(Player *player, float dt) {
    snapAxisToGrid(&player->position.x, player->tilePosition.x, dt);
}

static inline void snapToVerticalGrid(Player *player, float dt) {
    snapAxisToGrid(&player->position.y, player->tilePosition.y, dt);
}

static inline void snapToGridImmediate(Player *player) {
    player->position.x = (float) player->tilePosition.x;
    player->position.y = (float) player->tilePosition.y;
}

static void updateTilePosition(Player *player) {
    player->tilePosition.x = (int) roundf(player->position.x);
    player->tilePosition.y = (int) roundf(player->position.y);
}

void Player_init(Player *this) {
    this->state = PlayerState_None;

    this->position.x = 0;
    this->position.y = 0;

    updateTilePosition(this);
}

void Player_destroy(Player *this) {

}

// TODO: queue for direction keys
// TODO: replace this ugly ifs with something pretty
PlayerUpdateResult Player_update(Player *this, float dt, const PlayerInputState *inputState, Tilemap *map) {
    PlayerUpdateResult result = {
            .type = PlayerUpdateResultType_None
    };

    TileType currentTile = Tilemap_getTile(map, this->tilePosition).type;
    TileType floorTile = Tilemap_getTileWithOffset(map, this->tilePosition, 0, 1).type;
    TileType upperTile = Tilemap_getTileWithOffset(map, this->tilePosition, 0, -1).type;

    bool onFloor = isFloorTile(floorTile) || currentTile == TileType_Rope || currentTile == TileType_Ladder;

    // falling

    if (!(onFloor || this->state == PlayerState_FallingByRope))
        this->state = PlayerState_Falling;

    if (this->state == PlayerState_Falling || this->state == PlayerState_FallingByRope) {
        snapToHorizontalGrid(this, dt);
        this->position.y += dt * PLAYER_FALLING_SPEED;

        if (this->state == PlayerState_FallingByRope && currentTile == TileType_Rope &&
            this->ropeFallRow == this->tilePosition.y)
            onFloor = false;

        if (onFloor && (this->position.y >= (float) this->tilePosition.y)) {
            this->position.y = floorf(this->position.y);
            this->state = PlayerState_None;
        }

        updateTilePosition(this);

        return result;
    }

    // digging

    if (inputState->digLeft || inputState->digRight) {
        int dx = inputState->digRight ? 1 : -1;

        TileType digUpperTile = Tilemap_getTileWithOffset(map, this->tilePosition, dx, 0).type;
        TileType digTile = Tilemap_getTileWithOffset(map, this->tilePosition, dx, 1).type;

        if (digTile == TileType_Ground && digUpperTile == TileType_Air) {
            result.type = PlayerUpdateResultType_Dig;

            result.position.x = dx;
            result.position.y = 1;
            IVec2_add(&result.position, &this->tilePosition);

            snapToGridImmediate(this);
        }

        return result;
    }

    // vertical movement

    if (inputState->up) {
        if (currentTile == TileType_Ladder || floorTile == TileType_Ladder) {
            this->position.y -= dt * PLAYER_MOVEMENT_SPEED;

            if (!currentTile || isSolidTile(upperTile))
                this->position.y = maxf(this->position.y, (float) this->tilePosition.y);

            if (this->position.y != (float) this->tilePosition.y) {
                snapToHorizontalGrid(this, dt);
                updateTilePosition(this);
                return result;
            }
        }
    } else if (inputState->down) {
        this->position.y += dt * PLAYER_MOVEMENT_SPEED;

        if (currentTile == TileType_Rope && !isFloorTile(floorTile)) {
            this->state = PlayerState_FallingByRope;
            this->ropeFallRow = this->tilePosition.y;
            return result;
        }

        if (isSolidTile(floorTile))
            this->position.y = minf(this->position.y, (float) this->tilePosition.y);

        if (this->position.y != (float) this->tilePosition.y) {
            snapToHorizontalGrid(this, dt);
            updateTilePosition(this);
            return result;
        }
    }

    // horizontal movement

    if (inputState->left || inputState->right) {
        float dx = dt * (inputState->right ? PLAYER_MOVEMENT_SPEED : -PLAYER_MOVEMENT_SPEED);

        IVec2 newTilePos = {
                .x = (int) (this->position.x + dx),
                .y = this->tilePosition.y
        };

        if (inputState->right)
            newTilePos.x += 1;

        TileType tile = Tilemap_getTile(map, newTilePos).type;

        if (isSolidTile(tile)) {
            float nx = this->position.x + dx;

            if (!inputState->right)
                nx += 1.f;

            this->position.x = floorf(nx);
        } else {
            this->position.x += dx;
        }

        if (this->position.x != (float) this->tilePosition.x)
            snapToVerticalGrid(this, dt);
    }

    updateTilePosition(this);

    return result;
}

void Player_setPosition(Player *this, float x, float y) {
    this->position.x = x;
    this->position.y = y;

    updateTilePosition(this);
}

void Player_render(Player *this) {
    Vec2 pos = this->position;
//    IVec2 tpos = this->tilePosition;

    glBegin(GL_QUADS);

//    glColor3f(1.f, 0.f, 0.f);
//
//    glVertex2i(tpos.x, tpos.y);
//    glVertex2i(tpos.x, tpos.y + 1);
//    glVertex2i(tpos.x + 1, tpos.y + 1);
//    glVertex2i(tpos.x + 1, tpos.y);
//
//    glColor3f(1.f, 1.f, 1.f);

    glVertex2f(pos.x, pos.y);
    glVertex2f(pos.x, pos.y + 1.f);
    glVertex2f(pos.x + 1.f, pos.y + 1.f);
    glVertex2f(pos.x + 1.f, pos.y);

    glEnd();
}