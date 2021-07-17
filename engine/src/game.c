#include "game.h"

#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "sample_map.h"
#include "tileset.h"
#include "graphics/texture_loader.h"

#define TREASURE_ANIMATION_SPEED 15.f

static void processTileUpdates(Game *this, float dt) {
    LIST_PFOR(this->tileUpdates, it) {
        DeferredTileUpdate *update = &LIST_ITERATOR_VALUE(it);

        update->timeLeft -= dt;

        if (update->timeLeft <= 0) {
            Tilemap_setTile(&this->map, update->position, update->tileType);
            LIST_PFOR_REMOVE(this->tileUpdates, it)
        }

        LIST_PFOR_TAIL(it)
    }
}

static void fillTreasureList(Game *this) {
    IVec2 pos;

    for (pos.y = 0; pos.y < this->map.height; pos.y++) {
        for (pos.x = 0; pos.x < this->map.width; pos.x++) {
            if (Tilemap_getTile(&this->map, pos).type != TileType_Treasure)
                continue;

            Treasure *treasure = TreasureList_newItem(&this->treasures);
            treasure->position = pos;
            treasure->animationFrame = (float) (rand() % this->treasureFrameCount);
        }
    }
}

static void updateTreasures(Game *this, float dt) {
    LIST_FOR(this->treasures, it) {
        Treasure *treasure = &LIST_ITERATOR_VALUE(it);

        Tilemap_setTileVariant(&this->map, treasure->position, (uint8_t) treasure->animationFrame);

        treasure->animationFrame =
                fmodf(treasure->animationFrame + dt * TREASURE_ANIMATION_SPEED, this->treasureFrameCount);
    }
}

// TODO: create separate layer for treasures behind main layer
void Game_init(Game *this, VfsPackage *vfsPackage) {
    Tilemap_init(&this->map);
    Tilemap_assignTiles(&this->map, SAMPLE_MAP_WIDTH, SAMPLE_MAP_HEIGHT, SAMPLE_MAP_TILES);

    const TilemapRendererConfig *rendererConfig = Tileset_getRendererConfig();

    TilemapRenderer_init(
            &this->mapRenderer,
            GlTextureLoader_loadFromVfs(vfsPackage, "textures/atlas.png"),
            rendererConfig
    );

    this->treasureFrameCount = TilemapRendererConfig_getTileFrameCount(rendererConfig, TileType_Treasure);

    Player_init(&this->player);
    Player_setPosition(&this->player, 12.f, 8.f);

    DeferredTileUpdateList_init(&this->tileUpdates);

    TreasureList_init(&this->treasures);

    fillTreasureList(this);
}

void Game_destroy(Game *this) {
    TreasureList_destroy(&this->treasures);
    DeferredTileUpdateList_destroy(&this->tileUpdates);
    TilemapRenderer_destroy(&this->mapRenderer);
    Tilemap_destroy(&this->map);
}

void Game_update(Game *this, Window *window, float dt) {
    PlayerInputState playerInputState = {
            .up = Window_isKeyPressed(window, 0x26),
            .down = Window_isKeyPressed(window, 0x28),
            .left = Window_isKeyPressed(window, 0x25),
            .right = Window_isKeyPressed(window, 0x27),
            .digLeft = Window_isKeyPressed(window, 0x5a),
            .digRight = Window_isKeyPressed(window, 0x58)
    };

    PlayerUpdateResult result = Player_update(&this->player, dt, &playerInputState, &this->map);

    if (result.type == PlayerUpdateResultType_Dig) {
        TileType tileType = Tilemap_getTile(&this->map, result.position).type;

        Tilemap_setTile(&this->map, result.position, TileType_Air);

        DeferredTileUpdate *update = DeferredTileUpdateList_newItem(&this->tileUpdates);
        update->timeLeft = 5.f;
        update->position = result.position;
        update->tileType = tileType;
    }

    processTileUpdates(this, dt);
    updateTreasures(this, dt);
}

void Game_render(Game *this) {
    glClearColor(0.443f, 0.482f, 0.498f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLoadIdentity();
    gluOrtho2D(0, this->map.width, this->map.height, 0);

    TilemapRenderer_render(&this->mapRenderer, &this->map);
    Player_render(&this->player);

    glDisable(GL_BLEND);
}