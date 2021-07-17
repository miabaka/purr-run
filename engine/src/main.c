#include "system/window.h"
#include "game.h"
#include "vfs/package.h"

static void update(Window *window, float dt) {
    Game *game = Window_getUserPointer(window);
    Game_update(game, window, dt);
}

static void render(Window *window) {
    Game *game = Window_getUserPointer(window);
    Game_render(game);
}

int main() {
    VfsPackage package;

    VfsResult initResult = VfsPackage_init(&package, "data.pr") != VfsResult_Success;

    if (initResult != VfsResult_Success) {
        fprintf_s(stderr, "VfsPackage_init failed with code %u\n", initResult);
        return 1;
    }

    Window window;
    Window_init(&window, 640, 480, "Engine");

    Game game;
    Game_init(&game, &package);

    Window_setUserPointer(&window, &game);
    Window_setUpdateCallback(&window, update);
    Window_setRenderCallback(&window, render);

    Window_run(&window);

    Game_destroy(&game);
    Window_destroy(&window);

    VfsPackage_destroy(&package);

    return 0;
}