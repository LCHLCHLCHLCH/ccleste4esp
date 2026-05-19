#ifndef ESP32_FRONTEND_H
#define ESP32_FRONTEND_H

#include "celeste.h"

// PICO-8 模拟回调 — 传给 Celeste_P8_set_call_func
int pico8_emu(CELESTE_P8_CALLBACK_TYPE call, ...);

// Celeste_P8_draw() 后调用，将 framebuffer 推送到 LCD
void celeste_render_finish(void);

#endif
