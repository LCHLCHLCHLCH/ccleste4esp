"""
将所有字符的像素向上移动 1 像素
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
    
    # 向上移动: 把第1-5行的像素移到第0-4行
    # 第0行移到最顶上会消失, 第5行空出来
    for y in range(5):  # 0..4
        for x in range(4):
            pixels[cx + x, cy + y] = pixels[cx + x, cy + y + 1]
            # 右边镜像
            pixels[cx + x + 4, cy + y] = pixels[cx + x + 4, cy + y + 1]
    # 第5行清空
    for x in range(8):
        pixels[cx + x, cy + 5] = (200, 200, 200)

img.save("font.bmp")
print("已将所有字符上移 1 像素，保存到 font.bmp")
