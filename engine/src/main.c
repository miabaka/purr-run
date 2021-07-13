#include "system/window.h"
#include "game.h"

static void update(Window *window, float dt) {
    Game *game = Window_getUserPointer(window);
    Game_update(game, window, dt);
}

static void render(Window *window) {
    Game *game = Window_getUserPointer(window);
    Game_render(game);
}

int main() {
    Window window;
    Window_init(&window, 640, 480, "Engine");

    Game game;
    Game_init(&game);

    Window_setUserPointer(&window, &game);
    Window_setUpdateCallback(&window, update);
    Window_setRenderCallback(&window, render);

    Window_run(&window);

    Game_destroy(&game);
    Window_destroy(&window);

    return 0;
}