#include "timer.h"

#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

struct {
    uint64_t frequency;
    bool hasPc;
} g_timer;

void timer_init() {
    if (QueryPerformanceFrequency((LARGE_INTEGER *) &g_timer.frequency)) {
        g_timer.hasPc = true;
    } else {
        g_timer.frequency = 1000;
        g_timer.hasPc = false;
    }
}

double timer_getValue() {
    uint64_t value;

    if (g_timer.hasPc)
        QueryPerformanceCounter((LARGE_INTEGER *) &value);
    else
        value = timeGetTime();

    return (double) value / (double) g_timer.frequency;
}