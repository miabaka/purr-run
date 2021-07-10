#define GLFW_INCLUDE_NONE

#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "tilemap.h"
#include "graphics/tilemap_renderer.h"
#include "sample_map.h"
#include "math/vec2.h"
#include "math/util.h"
#include "tileset.h"

typedef enum {
    EntityState_None,
    EntityState_Falling,
    EntityState_FallingByRope
} EntityState;

typedef struct {
    Vec2 position;
    IVec2 tilePosition;
    int ropeFallRow;
    EntityState state;
} Entity;

void Entity_updateTilePosition(Entity *this) {
    this->tilePosition.x = (int) roundf(this->position.x);
    this->tilePosition.y = (int) roundf(this->position.y);
}

void drawPlayer(Entity *player) {
    Vec2 pos = player->position;
    IVec2 tpos = player->tilePosition;
//    Vec2_add(&pos, &player->offset);

    glBegin(GL_QUADS);

//    glColor3f(1.f, 0.f, 0.f);
//
//    glVertex2i(tpos.x, tpos.y);
//    glVertex2i(tpos.x, tpos.y + 1);
//    glVertex2i(tpos.x + 1, tpos.y + 1);
//    glVertex2i(tpos.x + 1, tpos.y);

    glColor3f(1.f, 1.f, 1.f);

    glVertex2f(pos.x, pos.y);
    glVertex2f(pos.x, pos.y + 1.f);
    glVertex2f(pos.x + 1.f, pos.y + 1.f);
    glVertex2f(pos.x + 1.f, pos.y);

    glEnd();
}

typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
    bool digLeft;
    bool digRight;
} PlayerInputState;

#define PLAYER_MOVEMENT_SPEED 4.f
#define PLAYER_FALLING_SPEED 7.f
#define PLAYER_SNAPPING_RATE 60.f

static inline bool tileIsSolid(Tile tile) {
    return tile == Tile_Ground || tile == Tile_Concrete || tile == Tile_Ice;
}

static inline bool tileIsFloor(Tile tile) {
    return tileIsSolid(tile) || tile == Tile_Ladder;
}

static void snapPlayerAxisToGrid(float *pos, int tilePos, float dt) {
    *pos = lerpf((float) tilePos, *pos, exp2f(-PLAYER_SNAPPING_RATE * dt));
}

static inline void snapPlayerToHorizontalGrid(Entity *player, float dt) {
    snapPlayerAxisToGrid(&player->position.x, player->tilePosition.x, dt);
}

static inline void snapPlayerToVerticalGrid(Entity *player, float dt) {
    snapPlayerAxisToGrid(&player->position.y, player->tilePosition.y, dt);
}

// TODO: queue for direction keys
// TODO: replace this ugly ifs with something pretty
void updatePlayer(Entity *player, float dt, PlayerInputState *inputState, Tilemap *map) {
    Tile currentTile = Tilemap_getTile(map, player->tilePosition);
    Tile floorTile = Tilemap_getTileWithOffset(map, player->tilePosition, 0, 1);
    Tile upperTile = Tilemap_getTileWithOffset(map, player->tilePosition, 0, -1);

    // falling

    bool onFloor = tileIsFloor(floorTile) || currentTile == Tile_Rope || currentTile == Tile_Ladder;

    if (!(onFloor || player->state == EntityState_FallingByRope))
        player->state = EntityState_Falling;

    if (player->state == EntityState_Falling || player->state == EntityState_FallingByRope) {
        snapPlayerToHorizontalGrid(player, dt);
        player->position.y += dt * PLAYER_FALLING_SPEED;

        if (player->state == EntityState_FallingByRope && currentTile == Tile_Rope &&
            player->ropeFallRow == player->tilePosition.y)
            onFloor = false;

        if (onFloor && (player->position.y >= (float) player->tilePosition.y)) {
            player->position.y = floorf(player->position.y);
            player->state = EntityState_None;
        }

        Entity_updateTilePosition(player);

        return;
    }

    // vertical movement

    if (inputState->up) {
        if (currentTile == Tile_Ladder || floorTile == Tile_Ladder) {
            player->position.y -= dt * PLAYER_MOVEMENT_SPEED;

            if (!currentTile || tileIsSolid(upperTile))
                player->position.y = maxf(player->position.y, (float) player->tilePosition.y);

            if (player->position.y != (float) player->tilePosition.y) {
                snapPlayerToHorizontalGrid(player, dt);
                Entity_updateTilePosition(player);
                return;
            }
        }
    } else if (inputState->down) {
        player->position.y += dt * PLAYER_MOVEMENT_SPEED;

        if (currentTile == Tile_Rope && !tileIsFloor(floorTile)) {
            player->state = EntityState_FallingByRope;
            player->ropeFallRow = player->tilePosition.y;
            return;
        }

        if (tileIsSolid(floorTile))
            player->position.y = minf(player->position.y, (float) player->tilePosition.y);

        if (player->position.y != (float) player->tilePosition.y) {
            snapPlayerToHorizontalGrid(player, dt);
            Entity_updateTilePosition(player);
            return;
        }
    }

    // horizontal movement

    if (inputState->left || inputState->right) {
        float dx = dt * (inputState->right ? PLAYER_MOVEMENT_SPEED : -PLAYER_MOVEMENT_SPEED);

        IVec2 newTilePos = {
                .x = (int) (player->position.x + dx),
                .y = player->tilePosition.y
        };

        if (inputState->right)
            newTilePos.x += 1;

        Tile tile = Tilemap_getTile(map, newTilePos);

        if (tileIsSolid(tile)) {
            float nx = player->position.x + dx;

            if (!inputState->right)
                nx += 1.f;

            player->position.x = floorf(nx);
        } else {
            player->position.x += dx;
        }

        if (player->position.x != (float) player->tilePosition.x)
            snapPlayerToVerticalGrid(player, dt);
    }

    Entity_updateTilePosition(player);
}

static void loadSampleMap(Tilemap *map) {
    Tilemap_assignTiles(map, SAMPLE_MAP_WIDTH, SAMPLE_MAP_HEIGHT, SAMPLE_MAP_TILES);
}

static void handleKeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        fputs(
                "\r| Resetting the map        |\n"
                "\\--------------------------/",
                stdout
        );

        Tilemap *map = glfwGetWindowUserPointer(window);
        loadSampleMap(map);
    }
}

int main() {
    fputs(
            "/-------= Controls =-------\\\n"
            "| arrows : movement        |\n"
            "| z/x    : dig left/right  |\n"
            "| r      : reset the map   |\n"
            "\\--------------------------/\n"
            "\n"
            "/-------= Messages =-------\\\n"
            "| H-hewwo?!                |\n"
            "\\--------------------------/",
            stdout
    );

    glfwInit();

    GLFWwindow *window = glfwCreateWindow(640, 480, "Engine", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glClearColor(0.443f, 0.482f, 0.498f, 1.f);

    gluOrtho2D(0, SAMPLE_MAP_WIDTH, SAMPLE_MAP_HEIGHT, 0);

    Tilemap map;
    Tilemap_init(&map);

    loadSampleMap(&map);

    glfwSetWindowUserPointer(window, &map);
    glfwSetKeyCallback(window, handleKeyEvent);

    TilemapRenderer tilemapRenderer;
    TilemapRenderer_init(&tilemapRenderer, "data/atlas.png", &TILESET_RENDERER_CONFIG);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Entity player = {
            .position = {10, 8}
    };

    Entity_updateTilePosition(&player);

    PlayerInputState playerInputState;

    double previousTime = 0.f;

    do {
        glfwPollEvents();

        double time = glfwGetTime();
        double timeDelta = time - previousTime;

        previousTime = time;

        playerInputState.up = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
        playerInputState.down = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
        playerInputState.left = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
        playerInputState.right = (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
        playerInputState.digLeft = (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS);
        playerInputState.digRight = (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS);

        updatePlayer(&player, (float) timeDelta, &playerInputState, &map);

        glClear(GL_COLOR_BUFFER_BIT);

        TilemapRenderer_render(&tilemapRenderer, &map);
        drawPlayer(&player);

        glfwSwapBuffers(window);
    } while (!glfwWindowShouldClose(window));

    TilemapRenderer_destroy(&tilemapRenderer);
    Tilemap_destroy(&map);

    glfwTerminate();

    puts(
            "\r| Bye!                     |\n"
            "\\--------------------------/"
    );

    return 0;
}