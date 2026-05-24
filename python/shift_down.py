"""
撤销上移 — 将所有字符像素向下移动 1 像素
"""
from PIL import Image

img = Image.open("font.bmp").convert("RGB")
w, h = img.size
pixels = img.load()

CELL = 8
COLS = w // CELL
ROWS = h // CELL

for i in range(95):
    col = i % COLS
    row = i // COLS
    cx = col * CELL
    cy = row * CELL
    
    # 向下移动: 第0-4行移到第1-5行, 第0行清空
    for y in range(4, -1, -1):  # 4,3,2,1,0
        for x in range(4):
            pixels[cx + x, cy + y + 1] = pixels[cx + x, cy + y]
            pixels[cx + x + 4, cy + y + 1] = pixels[cx + x + 4, cy + y]
    # 第0行清空
    for x in range(8):
        pixels[cx + x, cy + 0] = (200, 200, 200)

img.save("font.bmp")
print("已撤销 — 所有字符下移回原位")
