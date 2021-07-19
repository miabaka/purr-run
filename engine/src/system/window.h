#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../math/ivec2.h"

typedef struct Window Window;

typedef void (*Window_update_fun)(Window *window, float dt);

typedef void (*Window_render_fun)(Window *window);

typedef void (*Window_resize_fun)(Window *window, IVec2 size);

struct Window {
    void *handle;
    void *dc;
    void *userPointer;

    bool keys[256];

    struct {
        Window_update_fun update;
        Window_render_fun render;
        Window_resize_fun resize;
    } callbacks;
};

void Window_init(Window *this, int width, int height, const char *title);

void Window_destroy(Window *this);

void Window_run(Window *this);

void Window_updateSize(Window *this);

void Window_render(Window *this, bool flush);

static inline void *Window_getUserPointer(Window *this) {
    return this->userPointer;
}

static inline void Window_setUserPointer(Window *this, void *userPointer) {
    this->userPointer = userPointer;
}

static inline void Window_setUpdateCallback(Window *this, Window_update_fun callback) {
    this->callbacks.update = callback;
}

static inline void Window_setRenderCallback(Window *this, Window_render_fun callback) {
    this->callbacks.render = callback;
}

static inline void Window_setResizeCallback(Window *this, Window_resize_fun callback) {
    this->callbacks.resize = callback;
}

static inline bool Window_isKeyPressed(Window *this, uint8_t code) {
    return this->keys[code];
}