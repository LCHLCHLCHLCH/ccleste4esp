// ESP32 前端 — 实现 ccleste 的 PICO-8 回调
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "celeste.h"
#include "lcd.h"
#include "lcd_init.h"
#include "sprite.h"
#include "map_data.h"
#include "Drivers/button/button.h"
#include "driver/gpio.h"

extern spi_device_handle_t spi_port;

// ---- 全局状态 ----

uint8_t pico8_display_memory[128][128];  // PICO-8 颜色索引 framebuffer
extern uint16_t display_mem[128][128];   // LCD RGB565 framebuffer

static int camera_x = 0, camera_y = 0;
static uint8_t pal_map[16];              // 调色板映射
static int frame_count = 0;

// ---- PICO-8 → 标准 RGB565 调色板 ----
static const uint16_t pico8_rgb565[16] = {
    0x0000, 0x194A, 0x792A, 0x042A,
    0xAA86, 0x5AA9, 0xC618, 0xFF9D,
    0xF809, 0xFD00, 0xFF64, 0x0726,
    0x2D7F, 0x83B3, 0xFBB5, 0xFE75,
};

// ---- 精灵 flag 表 (来自 ccleste/tilemap.h) ----
static const uint8_t sprite_flags[128] = {
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    4,2,0,0,0,0,0,0, 0,0,0,2,0,0,0,0,
    3,3,3,3,3,3,3,3, 4,4,4,2,2,0,0,0,
    3,3,3,3,3,3,3,3, 4,4,4,2,2,2,2,2,
    0,0,19,19,19,19,2,2, 3,2,2,2,2,2,0,2,
    0,0,19,19,19,19,2,2, 4,2,2,2,2,2,2,2,
    0,0,19,19,19,19,0,4, 4,2,2,2,2,2,2,2,
    0,0,19,19,19,19,0,0, 0,2,2,2,2,2,2,2,
};

// ---- 4×5 位图字体 (ASCII 32-126) ----
static const uint8_t p8_font[95][5] = {
    {0x0,0x0,0x0,0x0,0x0},{0x2,0x2,0x2,0x0,0x2},{0x5,0x5,0x0,0x0,0x0},{0x5,0xF,0x5,0xF,0x5},
    {0x6,0x3,0x2,0x6,0x3},{0x9,0x8,0x4,0x2,0x9},{0x6,0x9,0x5,0xA,0xD},{0x2,0x2,0x0,0x0,0x0},
    {0x4,0x2,0x2,0x2,0x4},{0x2,0x4,0x4,0x4,0x2},{0x0,0x5,0x2,0x5,0x0},{0x0,0x4,0xE,0x4,0x0},
    {0x0,0x0,0x0,0x2,0x1},{0x0,0x0,0xF,0x0,0x0},{0x0,0x0,0x0,0x0,0x2},{0x8,0x4,0x2,0x1,0x0},
    {0x6,0x9,0x9,0x9,0x6},{0x4,0x6,0x4,0x4,0xE},{0x6,0x9,0x4,0x2,0xF},{0x6,0x9,0x4,0x9,0x6},
    {0x8,0xC,0xA,0xF,0x8},{0xF,0x1,0x7,0x8,0x7},{0x6,0x1,0x7,0x9,0x6},{0xF,0x8,0x4,0x2,0x2},
    {0x6,0x9,0x6,0x9,0x6},{0x6,0x9,0xE,0x8,0x6},{0x0,0x2,0x0,0x2,0x0},{0x0,0x2,0x0,0x2,0x1},
    {0x8,0x4,0x2,0x4,0x8},{0x0,0xF,0x0,0xF,0x0},{0x2,0x4,0x8,0x4,0x2},{0x6,0x9,0x4,0x0,0x4},
    {0x6,0xD,0xD,0x1,0x6},{0x6,0x9,0xF,0x9,0x9},{0x7,0x9,0x7,0x9,0x7},{0x6,0x9,0x1,0x9,0x6},
    {0x7,0x9,0x9,0x9,0x7},{0xF,0x1,0x7,0x1,0xF},{0xF,0x1,0x7,0x1,0x1},{0x6,0x9,0xD,0x9,0x6},
    {0x9,0x9,0xF,0x9,0x9},{0xE,0x4,0x4,0x4,0xE},{0xE,0x8,0x8,0x9,0x6},{0x9,0x5,0x3,0x5,0x9},
    {0x1,0x1,0x1,0x1,0xF},{0x9,0xF,0xF,0x9,0x9},{0x9,0xB,0xD,0x9,0x9},{0x6,0x9,0x9,0x9,0x6},
    {0x7,0x9,0x7,0x1,0x1},{0x6,0x9,0x9,0x5,0xA},{0x7,0x9,0x7,0x5,0x9},{0x6,0x9,0x2,0x9,0x6},
    {0xF,0x4,0x4,0x4,0x4},{0x9,0x9,0x9,0x9,0x6},{0x9,0x9,0x9,0x6,0x6},{0x9,0x9,0xF,0xF,0x9},
    {0x9,0x6,0x6,0x6,0x9},{0x9,0x9,0x6,0x4,0x4},{0xF,0x8,0x4,0x2,0xF},{0xE,0x2,0x2,0x2,0xE},
    {0x1,0x2,0x4,0x8,0x8},{0xE,0x8,0x8,0x8,0xE},{0x4,0xA,0x0,0x0,0x0},{0x0,0x0,0x0,0x0,0xF},
    {0x2,0x4,0x0,0x0,0x0},{0x0,0x6,0xA,0xA,0xD},{0x1,0x3,0x5,0x5,0x3},{0x0,0x6,0x1,0x1,0x6},
    {0x8,0xC,0xA,0xA,0xC},{0x0,0x6,0xF,0x1,0x6},{0x4,0xA,0x2,0x7,0x2},{0x0,0xC,0xA,0xC,0x8},
    {0x1,0x3,0x5,0x5,0x5},{0x0,0x2,0x0,0x2,0x2},{0x0,0x8,0x0,0x8,0x6},{0x1,0x5,0x3,0x5,0x5},
    {0x2,0x2,0x2,0x2,0x2},{0x0,0x5,0xF,0x9,0x9},{0x0,0x3,0x5,0x5,0x5},{0x0,0x6,0x9,0x9,0x6},
    {0x0,0x7,0x9,0x7,0x1},{0x0,0xC,0xA,0xC,0x8},{0x0,0xD,0x3,0x1,0x1},{0x0,0x6,0x2,0x4,0x6},
    {0x2,0x7,0x2,0x2,0x6},{0x0,0x9,0x9,0x9,0x6},{0x0,0x9,0x9,0x6,0x6},{0x0,0x9,0x9,0xF,0x6},
    {0x0,0x9,0x6,0x6,0x9},{0x0,0x9,0xA,0xC,0x8},{0x0,0xF,0x4,0x2,0xF},{0x8,0x4,0x2,0x4,0x8},
    {0x2,0x2,0x2,0x2,0x2},{0x2,0x4,0x8,0x4,0x2},{0x0,0x5,0xA,0x0,0x0},
};

// ---- 辅助函数 ----

static inline void draw_point(int px, int py, uint8_t color) {
    if (px >= 0 && px < 128 && py >= 0 && py < 128)
        display_mem[py][px] = pico8_rgb565[pal_map[color]];
}

// 将 pico8_display_memory 转换为 display_mem 并发送到 LCD
static void commit_framebuffer(void) {
    uint16_t *mem = (uint16_t *)display_mem;

    // DMA 字节交换
    for (int i = 0; i < 128 * 128; i++)
        mem[i] = __builtin_bswap16(mem[i]);

    LCD_Address_Set(0, 0, 127, 127);
    spi_transaction_t t = {0};
    t.length = 128 * 128 * 16;
    t.tx_buffer = (uint8_t *)display_mem;
    t.user = (void *)1;
    spi_device_polling_transmit(spi_port, &t);

    for (int i = 0; i < 128 * 128; i++)
        mem[i] = __builtin_bswap16(mem[i]);
}

// ---- PICO-8 回调实现 ----

int pico8_emu(CELESTE_P8_CALLBACK_TYPE call, ...) {
    va_list args;
    va_start(args, call);
    int ret = 0;

    switch (call) {
    case CELESTE_P8_MUSIC:  // no-op
        va_arg(args, int); va_arg(args, int); va_arg(args, int);
        break;
    case CELESTE_P8_SFX:    // no-op
        va_arg(args, int);
        break;
    case CELESTE_P8_SPR: {
        int sprite = va_arg(args, int);
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int cols = va_arg(args, int);
        int rows = va_arg(args, int);
        bool flipx = va_arg(args, int);
        bool flipy = va_arg(args, int);

        int p_x = sprite % 16;
        int p_y = sprite / 16;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                int sx = flipx ? (7 - i) : i;
                int sy = flipy ? (7 - j) : j;
                int dx = x + i - camera_x;
                int dy = y + j - camera_y;
                if (dx >= 0 && dx < 128 && dy >= 0 && dy < 128) {
                    uint8_t col = sprite_sheet[(p_y * 8 + sy)][(p_x * 8 + sx)];
                    if (col != 0) // 0 = 透明
                        display_mem[dy][dx] = pico8_rgb565[pal_map[col]];
                }
            }
        }
        break;
    }
    case CELESTE_P8_BTN: {
        int b = va_arg(args, int);
        switch (b) {
        case 0: ret = gpio_get_level(1) == 0; break;  // left
        case 1: ret = gpio_get_level(3) == 0; break;  // right
        case 2: ret = gpio_get_level(2) == 0; break;  // up
        case 3: ret = gpio_get_level(4) == 0; break;  // down
        case 4: ret = gpio_get_level(5) == 0; break;  // jump
        case 5: ret = gpio_get_level(6) == 0; break;  // dash
        default: ret = 0; break;
        }
        break;
    }
    case CELESTE_P8_PAL: {
        int a = va_arg(args, int);
        int b = va_arg(args, int);
        pal_map[a & 0xf] = b & 0xf;
        break;
    }
    case CELESTE_P8_PAL_RESET: {
        for (int i = 0; i < 16; i++) pal_map[i] = i;
        break;
    }
    case CELESTE_P8_CIRCFILL: {
        int X = va_arg(args, int);
        int Y = va_arg(args, int);
        int r = va_arg(args, int);
        int color = va_arg(args, int);

        int x = 0, y = r, d = 1 - r;
        draw_point(X + x, Y + y, color);
        draw_point(X - x, Y - y, color);
        for (int j = -y; j < y; j++) draw_point(X, Y + j, color);

        while (x < y) {
            x++;
            if (d < 0) d += 2 * x + 1;
            else { y--; d += 2 * (x - y) + 1; }

            draw_point(X + x, Y + y, color);
            draw_point(X + y, Y + x, color);
            draw_point(X - x, Y - y, color);
            draw_point(X - y, Y - x, color);
            draw_point(X + x, Y - y, color);
            draw_point(X + y, Y - x, color);
            draw_point(X - x, Y + y, color);
            draw_point(X - y, Y + x, color);

            for (int j = -y; j < y; j++) {
                draw_point(X + x, Y + j, color);
                draw_point(X - x, Y + j, color);
            }
            for (int j = -x; j < x; j++) {
                draw_point(X - y, Y + j, color);
                draw_point(X + y, Y + j, color);
            }
        }
        break;
    }
    case CELESTE_P8_PRINT: {
        const char *str = va_arg(args, const char *);
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int c = va_arg(args, int);

        while (*str) {
            char ch = *str++;
            if (ch < 32 || ch > 126) continue;
            int idx = ch - 32;
            for (int row = 0; row < 5; row++) {
                uint8_t bits = p8_font[idx][row];
                for (int col = 0; col < 4; col++) {
                    if (bits & (1 << (3 - col)))
                        draw_point(x + col - camera_x, y + row - camera_y, c);
                }
            }
            x += 4;
        }
        break;
    }
    case CELESTE_P8_RECTFILL: {
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int x2 = va_arg(args, int);
        int y2 = va_arg(args, int);
        int c = va_arg(args, int);

        if (x > x2) { int t = x; x = x2; x2 = t; }
        if (y > y2) { int t = y; y = y2; y2 = t; }

        // 裁剪
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x2 > 127) x2 = 127;
        if (y2 > 127) y2 = 127;

        uint16_t rgb = pico8_rgb565[pal_map[c]];
        for (int i = x; i <= x2; i++)
            for (int j = y; j <= y2; j++)
                display_mem[j][i] = rgb;
        break;
    }
    case CELESTE_P8_LINE: {
        int x0 = va_arg(args, int);
        int y0 = va_arg(args, int);
        int x1 = va_arg(args, int);
        int y1 = va_arg(args, int);
        int c = va_arg(args, int);

        // Bresenham 画线
        int dx = (x1 > x0) ? x1 - x0 : x0 - x1;
        int dy = (y1 > y0) ? y1 - y0 : y0 - y1;
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        while (1) {
            draw_point(x0, y0, c);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x0 += sx; }
            if (e2 < dx) { err += dx; y0 += sy; }
        }
        break;
    }
    case CELESTE_P8_MGET: {
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        ret = map_data[y * 128 + x];
        // -1 在 ESP32 地图中表示空格，转换为 0 匹配 ccleste 的行为
        if (ret < 0) ret = 0;
        break;
    }
    case CELESTE_P8_CAMERA: {
        camera_x = va_arg(args, int);
        camera_y = va_arg(args, int);
        break;
    }
    case CELESTE_P8_FGET: {
        int tile = va_arg(args, int);
        int flag = va_arg(args, int);
        if (tile < 0 || tile >= 128) ret = 0;
        else ret = (sprite_flags[tile] & (1 << flag)) != 0;
        break;
    }
    case CELESTE_P8_MAP: {
        int mx = va_arg(args, int);
        int my = va_arg(args, int);
        int tx = va_arg(args, int);
        int ty = va_arg(args, int);
        int mw = va_arg(args, int);
        int mh = va_arg(args, int);
        int mask = va_arg(args, int);

        for (int x = 0; x < mw; x++) {
            for (int y = 0; y < mh; y++) {
                int tile = map_data[(my + y) * 128 + (mx + x)];
                if (tile == -1) continue;

                bool draw = false;
                if (mask == 0) draw = true;
                else if (mask == 4 && sprite_flags[tile] == 4) draw = true;
                else if (mask != 4 && (sprite_flags[tile] & (1 << (mask - 1)))) draw = true;

                if (draw) {
                    int p_x = tile % 16;
                    int p_y = tile / 16;
                    for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {
                            int dx = tx + x * 8 + i - camera_x;
                            int dy = ty + y * 8 + j - camera_y;
                            if (dx >= 0 && dx < 128 && dy >= 0 && dy < 128) {
                                uint8_t col = sprite_sheet[(p_y * 8 + j)][(p_x * 8 + i)];
                                if (col != 0) // 0 = 透明
                                    display_mem[dy][dx] = pico8_rgb565[pal_map[col]];
                            }
                        }
                    }
                }
            }
        }
        break;
    }
    default:
        break;
    }

    va_end(args);
    return ret;
}

// ---- Celeste 调用的绘制完成函数 ----
// 替代原来的 display_all，在 Celeste_P8_draw 之后调用
void celeste_render_finish(void) {
    commit_framebuffer();
    frame_count++;
}
