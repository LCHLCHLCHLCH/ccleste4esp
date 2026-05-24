"""
生成空白字体模板 (所有字形为空白, 网格线 + 标签)
"""
from PIL import Image

CELL = 8
COLS = 16
ROWS = 6

img_w = COLS * CELL
img_h = ROWS * CELL
img = Image.new("RGB", (img_w, img_h), (255, 255, 255))
pixels = img.load()

for i in range(95):
    col = i % COLS
    row = i // COLS
    cx = col * CELL
    cy = row * CELL
    # 全部填充浅灰 (背景)
    for y in range(6):
        for x in range(4):
            pixels[cx + x,     cy + y] = (200, 200, 200)
            pixels[cx + x + 4, cy + y] = (200, 200, 200)
    # 网格线
    for x in range(CELL):
        pixels[cx + x, cy + 7] = (160, 160, 160)
    for y in range(CELL):
        pixels[cx + 7, cy + y] = (160, 160, 160)
    # 标签: 在底部像素下方写字符名
    name = chr(i + 32) if 32 <= i + 32 <= 126 else '?'
    # 简单占位 - 标签在 txt 文件中体现

img.save("font_blank.bmp")
print(f"生成 font_blank.bmp ({img_w}×{img_h}) — 空白模板，黑色画字形，灰色留空")
