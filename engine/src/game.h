#pragma once

#define GLFW_INCLUDE_NONE

#include <stdbool.h>
#include <GLFW/glfw3.h>
#include "math/vec2.h"
#include "math/ivec2.h"
#include "tilemap.h"
#include "graphics/tilemap_renderer.h"
#include "player.h"

typedef struct {
    Player player;
    Tilemap map;
    TilemapRenderer mapRenderer;
} Game;

void Game_init(Game *this);

void Game_destroy(Game *this);

void Game_update(Game *this, GLFWwindow *window, float dt);

void Game_render(Game *this);