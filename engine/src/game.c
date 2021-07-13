#include "game.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include "sample_map.h"
#include "tileset.h"

void Game_init(Game *this) {
    Tilemap_init(&this->map);
    Tilemap_assignTiles(&this->map, SAMPLE_MAP_WIDTH, SAMPLE_MAP_HEIGHT, SAMPLE_MAP_TILES);

    TilemapRenderer_init(&this->mapRenderer, "data/atlas.png", &TILESET_RENDERER_CONFIG);

    Player_init(&this->player);
    Player_setPosition(&this->player, 12.f, 8.f);

    DeferredTileUpdateList_init(&this->tileUpdateList);
}

void Game_destroy(Game *this) {
    DeferredTileUpdateList_destroy(&this->tileUpdateList);
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

        DeferredTileUpdate *update = DeferredTileUpdateList_newItem(&this->tileUpdateList);
        update->timeLeft = 5.f;
        update->position = result.position;
        update->tileType = tileType;
    }

    LIST_PFOR(this->tileUpdateList, it) {
        DeferredTileUpdate *update = &LIST_ITERATOR_VALUE(it);

        update->timeLeft -= dt;

        if (update->timeLeft <= 0) {
            Tilemap_setTile(&this->map, update->position, update->tileType);
            LIST_PFOR_REMOVE(this->tileUpdateList, it)
        }

        LIST_PFOR_TAIL(it)
    }
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