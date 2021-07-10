#define GLFW_INCLUDE_NONE

#include <stddef.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include "game.h"

int main() {
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_VISIBLE, false);

    GLFWwindow *window = glfwCreateWindow(640, 480, "Engine", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    Game game;
    Game_init(&game);

    glfwShowWindow(window);

    double previousTime = 0.f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double time = glfwGetTime();
        double timeDelta = time - previousTime;

        previousTime = time;

        Game_update(&game, window, (float) timeDelta);
        Game_render(&game);

        glfwSwapBuffers(window);
    }

    Game_destroy(&game);

    glfwTerminate();

    return 0;
}