#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct Window Window;

typedef void (*Window_update_fun)(Window *window, float dt);

typedef void (*Window_render_fun)(Window *window);

struct Window {
    void *handle;
    void *userPointer;

    bool keys[256];

    struct {
        Window_update_fun update;
        Window_render_fun render;
    } callbacks;
};

void Window_init(Window *this, int width, int height, const char *title);

void Window_destroy(Window *this);

void Window_run(Window *this);

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

static inline bool Window_isKeyPressed(Window *this, uint8_t code) {
    return this->keys[code];
}