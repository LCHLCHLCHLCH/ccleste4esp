from PIL import Image
import os

# PICO-8 调色板 (24-bit RGB)
pico8_palette = [
    (0,0,0),       # 0
    (29,43,83),    # 1
    (126,37,83),   # 2
    (0,135,81),    # 3
    (171,82,54),   # 4
    (95,87,79),    # 5
    (194,195,199), # 6
    (255,241,232), # 7
    (255,0,77),    # 8
    (255,163,0),   # 9
    (255,236,39),  # 10
    (0,228,54),    # 11
    (41,173,255),  # 12
    (131,118,156), # 13
    (255,119,168), # 14
    (255,204,170), # 15
]

# 把 RGB 映射到最近的 PICO-8 颜色
def nearest_p8(r, g, b):
    best = 0
    best_dist = 999999
    for i, (pr, pg, pb) in enumerate(pico8_palette):
        dist = (r-pr)**2 + (g-pg)**2 + (b-pb)**2
        if dist < best_dist:
            best_dist = dist
            best = i
    return best

# 屏幕公式: R(5):B(6):G(5)
def to_lcd(r, g, b):
    return ((r>>3)<<11) | ((b>>2)<<5) | (g>>3)

# 预计算 PICO-8 16 色在 LCD 上的值
lcd_palette = [to_lcd(r,g,b) for (r,g,b) in pico8_palette]

def convert(image_path, output_path):
    img = Image.open(image_path).convert("RGB").resize((128, 128))
    
    # 生成 4-bit 索引数组 (每个字节存两个像素)
    indices = bytearray()
    for y in range(128):
        for x in range(0, 128, 2):
            r1,g1,b1 = img.getpixel((x, y))
            r2,g2,b2 = img.getpixel((x+1, y))
            i1 = nearest_p8(r1, g1, b1)
            i2 = nearest_p8(r2, g2, b2)
            indices.append((i1 << 4) | i2)
    
    with open(output_path, "w") as f:
        f.write("#ifndef SCREENSHOT_DATA_H\n")
        f.write("#define SCREENSHOT_DATA_H\n")
        f.write("#include <stdint.h>\n")
        f.write("// 4-bit PICO-8 color indices, 2 pixels per byte\n")
        f.write(f"static const uint8_t screenshot_indices[{len(indices)}] = {{\n    ")
        for i, b in enumerate(indices):
            f.write(f"0x{b:02X}, ")
            if (i+1) % 16 == 0:
                f.write("\n    ")
        f.write("\n};\n")
        f.write(f"// LCD palette (corrected for R:B:G layout)\n")
        f.write(f"static const uint16_t lcd_palette[16] = {{\n    ")
        for i, val in enumerate(lcd_palette):
            f.write(f"0x{val:04X}, ")
        f.write("\n};\n")
        f.write("#endif\n")
    
    size_kb = len(indices) / 1024
    print(f"Generated {output_path}: {len(indices)} bytes ({size_kb:.1f} KB)")

if __name__ == "__main__":
    current_folder = os.path.dirname(os.path.abspath(__file__))
    convert(
        current_folder + "/screenshot.png",
        current_folder + "/../main/Drivers/LCD/screenshot_data.h"
    )
