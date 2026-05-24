"""
image_to_font.py — 从 BMP 字体图片生成 C 字体头文件
输入: font_preview.bmp (16列×6行, 每格8×8, 字体在左上4×6区域)
输出: font_data.h
用法: python image_to_font.py [input.bmp] [output.h]
"""
from PIL import Image
import sys

def main():
    img_path = "font_preview.bmp"
    out_path = "main/font_data.h"
    if len(sys.argv) > 1:
        img_path = sys.argv[1]
    if len(sys.argv) > 2:
        out_path = sys.argv[2]

    img = Image.open(img_path).convert("L")  # 灰度
    w, h = img.size
    pixels = img.load()

    CELL = 8
    COLS = w // CELL
    ROWS = h // CELL
    
    chars = []
    for i in range(95):  # ASCII 32-126
        col = i % COLS
        row = i // COLS
        cx = col * CELL
        cy = row * CELL
        rows = []
        for y in range(6):
            val = 0
            for x in range(4):
                # 像素亮度 < 128 视为有色 (黑像素)
                if pixels[cx + x, cy + y] < 128:
                    val |= (1 << (3 - x))
            rows.append(val)
        chars.append(rows)

    # 验证 'A'
    print("'A' (idx 33):")
    for r in chars[33]:
        s = "".join("#" if (r >> (3 - b)) & 1 else "." for b in range(4))
        print(f"  0x{r:02X} = {s}")

    out = """#ifndef FONT_DATA_H
#define FONT_DATA_H
#include <stdint.h>
// PICO-8 4x6 font (generated from font bitmap)
// 95 chars ASCII 32-126, each 6 rows of 4 bits
static const uint8_t p8_font[95][6] = {
"""
    for i, ch in enumerate(chars):
        name = chr(i + 32) if 32 <= i + 32 <= 126 else '?'
        esc = '\\\\' if name == '\\' else (name if name != "'" else "\\'")
        out += f"    {{0x{ch[0]:02X},0x{ch[1]:02X},0x{ch[2]:02X},0x{ch[3]:02X},0x{ch[4]:02X},0x{ch[5]:02X}}}, // '{esc}'\n"
    out += "};\n#endif\n"

    with open(out_path, "w") as f:
        f.write(out)
    print(f"\n写入 {out_path} ({len(chars)} 字符)")

if __name__ == "__main__":
    main()
