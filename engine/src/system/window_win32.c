#include "window.h"

#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <dwmapi.h>
#include "timer.h"

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x0001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x0002

typedef HGLRC WINAPI (*wglCreateContextAttribsARB_fun)(HDC hDC, HGLRC hshareContext, const int *attribList);

typedef BOOL WINAPI (*wglSwapIntervalEXT_fun)(int interval);

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    Window *this = GetProp(window, "prwnd");

    switch (msg) {
        case WM_KEYDOWN:
        case WM_KEYUP:
            if (wParam < 256) {
                this->keys[wParam] = !(HIWORD(lParam) & KF_UP);
                return 0;
            }

            break;

        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;

            break;

        default:
            break;
    }

    return DefWindowProc(window, msg, wParam, lParam);
}

static void createContext(HWND window) {
    HDC dc = GetDC(window);

    PIXELFORMATDESCRIPTOR pfd = {
            sizeof(pfd),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,
            8,
            0,
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
    };

    SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);

    HGLRC dummyContext = wglCreateContext(dc);
    wglMakeCurrent(dc, dummyContext);

    wglCreateContextAttribsARB_fun _wglCreateContextAttribsARB =
            (wglCreateContextAttribsARB_fun) wglGetProcAddress("wglCreateContextAttribsARB");

    const int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
            0
    };

    HGLRC context = _wglCreateContextAttribsARB(dc, NULL, attribs);
    wglMakeCurrent(dc, context);

    ((wglSwapIntervalEXT_fun) wglGetProcAddress("wglSwapIntervalEXT"))(0);

    wglDeleteContext(dummyContext);
}

// FIXME: non-unique class name
// TODO: create class once at app startup
void Window_init(Window *this, int width, int height, const char *title) {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS windowClass = {
            .hInstance = hInstance,
            .lpszClassName = "render",
            .lpfnWndProc = windowProc,
            .style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW,
            .hCursor = LoadCursor(NULL, IDC_ARROW),
            .hbrBackground = GetStockObject(BLACK_BRUSH)
    };

    RegisterClass(&windowClass);

    RECT windowRect = {
            .right = width,
            .bottom = height
    };

    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND window = CreateWindow(
            "render",
            title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            NULL,
            NULL,
            hInstance,
            0
    );

    SetProp(window, "prwnd", this);

    createContext(window);

    this->handle = window;
    this->userPointer = NULL;
    this->callbacks.update = NULL;
    this->callbacks.render = NULL;

    memset(this->keys, 0, sizeof(this->keys));
}

void Window_destroy(Window *this) {
    RemoveProp(this->handle, "prwnd");
    DestroyWindow(this->handle);
    UnregisterClass("render", GetModuleHandle(NULL));
}

void Window_run(Window *this) {
    timer_init();

    ShowWindow(this->handle, SW_SHOW);

    HDC dc = GetDC(this->handle);

    MSG msg;
    double previousTime = 0.f;

    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                break;
        } else {
            double time = timer_getValue();
            double timeDelta = time - previousTime;

            previousTime = time;

            if (this->callbacks.update)
                this->callbacks.update(this, (float) timeDelta);

            if (this->callbacks.render)
                this->callbacks.render(this);

            DwmFlush();
            SwapBuffers(dc);
        }
    }
}