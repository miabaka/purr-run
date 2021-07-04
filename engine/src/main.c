#define GLFW_INCLUDE_NONE

#include <stddef.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "tilemap.h"
#include "graphics/tilemap_renderer.h"
#include "sample_map.h"

int main() {
    glfwInit();

    GLFWwindow *window = glfwCreateWindow(640, 480, "Engine", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glClearColor(0.443f, 0.482f, 0.498f, 1.f);

    gluOrtho2D(0, SAMPLE_MAP_WIDTH, SAMPLE_MAP_HEIGHT, 0);

    MultiframeAtlasEntry tilesetAtlasEntry = {
            .x = 0,
            .y = 0,
            .firstFrame = 0,
            .frameWidth = 32,
            .frameHeight = 32,
            .framesPerRow = 8,
            .frameCount = 8
    };

    Tilemap map;
    Tilemap_init(&map);

    Tilemap_assignTiles(&map, SAMPLE_MAP_WIDTH, SAMPLE_MAP_HEIGHT, SAMPLE_MAP_TILES);

    TilemapRenderer tilemapRenderer;
    TilemapRenderer_init(&tilemapRenderer, "data/tileset.png", &tilesetAtlasEntry);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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