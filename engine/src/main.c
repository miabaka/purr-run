#define GLFW_INCLUDE_NONE

#include <stdint.h>
#include <stddef.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "graphics/tilemap_renderer.h"
#include "tile.h"
#include "tilemap.h"

#define SAMPLE_LEVEL_WIDTH 20
#define SAMPLE_LEVEL_HEIGHT 15
#define SAMPLE_LEVEL_SIZE (SAMPLE_LEVEL_WIDTH * SAMPLE_LEVEL_HEIGHT)

const Tile SAMPLE_LEVEL_TILES[SAMPLE_LEVEL_SIZE] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 5, 5, 5, 0, 2, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 1, 1, 4, 2, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 5, 5, 4, 0, 0, 0, 0, 4, 2, 0, 0, 0, 0,
        0, 1, 4, 1, 3, 3, 3, 0, 0, 4, 4, 0, 0, 0, 4, 0, 0, 2, 0, 0,
        0, 0, 4, 0, 0, 0, 0, 0, 0, 4, 4, 4, 0, 0, 4, 2, 1, 2, 0, 0,
        0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 2, 0, 0,
        0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 2, 2, 0, 0, 0,
        0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 2, 0, 0, 0, 0,
        1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 2, 0, 0, 0, 0,
        0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 2, 0, 0, 0, 0,
        0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2, 4, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 0
};

int main() {
    glfwInit();

    GLFWwindow *window = glfwCreateWindow(640, 480, "Engine", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glClearColor(0.2f, 0.1f, 0.3f, 1.f);

    gluOrtho2D(0, SAMPLE_LEVEL_WIDTH, SAMPLE_LEVEL_HEIGHT, 0);

    MultiframeAtlasEntry tilesetAtlasEntry = {
            .x = 0,
            .y = 0,
//            .firstFrame = 0,
            .frameWidth = 32,
            .frameHeight = 32,
            .framesPerRow = 8,
            .frameCount = 8
    };

    Tilemap map;
    Tilemap_init(&map);

    Tilemap_assignTiles(&map, SAMPLE_LEVEL_WIDTH, SAMPLE_LEVEL_HEIGHT, SAMPLE_LEVEL_TILES);

    TilemapRenderer tilemapRenderer;
    TilemapRenderer_init(&tilemapRenderer, "data/tileset.png", &tilesetAtlasEntry);

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        TilemapRenderer_render(&tilemapRenderer, &map);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));

    TilemapRenderer_destroy(&tilemapRenderer);
    Tilemap_destroy(&map);

    glfwTerminate();

    return 0;
}