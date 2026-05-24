"""
font_to_image.py — 从 C 字体数据生成可视化的 BMP 图片和 ASCII 文本对照
输出: font_preview.bmp (16列×6行, 每格8×8像素)
      font_preview.txt (16列网格对照, 与图片位置一一对应)
"""
from PIL import Image
import sys

def read_font_from_header(path):
    chars = []
    with open(path, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('{') and '//' in line:
                vals = line.split('{')[1].split('}')[0]
                rows = [int(x.strip(), 16) for x in vals.split(',')]
                chars.append(rows)
    return chars

def char_to_grid(ch_rows):
    """渲染单个字符为4×6的文本格子，每行是一个长度4的字符串"""
    lines = []
    for y in range(6):
        s = "".join("#" if (ch_rows[y] >> (3 - x)) & 1 else "." for x in range(4))
        lines.append(s)
    return lines

def main():
    header_path = "main/font_data.h"
    if len(sys.argv) > 1:
        header_path = sys.argv[1]
    
    chars = read_font_from_header(header_path)
    n = len(chars)
    print(f"读取 {n} 个字符")

    COLS = 16
    CELL = 8
    ROWS = (n + COLS - 1) // COLS
    
    img_w = COLS * CELL
    img_h = ROWS * CELL
    img = Image.new("RGB", (img_w, img_h), (255, 255, 255))
    pixels = img.load()

    # --- 生成 BMP ---
    for i, ch_rows in enumerate(chars):
        col = i % COLS
        row = i // COLS
        cx = col * CELL
        cy = row * CELL
        for y in range(6):
            bits = ch_rows[y]
            for x in range(4):
                if bits & (1 << (3 - x)):
                    pixels[cx + x,     cy + y] = (0, 0, 0)
                    pixels[cx + x + 4, cy + y] = (0, 0, 0)
                else:
                    pixels[cx + x,     cy + y] = (200, 200, 200)
                    pixels[cx + x + 4, cy + y] = (200, 200, 200)
        # 网格线
        for x in range(CELL):
            pixels[cx + x, cy + 7] = (180, 180, 180)
        for y in range(CELL):
            pixels[cx + 7, cy + y] = (180, 180, 180)

    img.save("font_preview.bmp")
    print(f"生成 font_preview.bmp ({img_w}×{img_h})")

    # --- 生成 TXT: 与图片位置完全对应 (16列网格) ---
    # 每个字符占 4×6 文本区域, 用一行6行×16列的方式渲染
    # 即: 第k个字符在第 row = k//16 行, col = k%16 列
    # 文本输出: 每行字符的同一行像素拼在一起, 字符间用空格分隔
    
    lines = []
    for grid_row in range(ROWS):
        # 该网格行有 COLS 个字符 (最后一行可能不够)
        row_chars = chars[grid_row * COLS : min((grid_row + 1) * COLS, n)]
        # 渲染每个字符成 4×6 文本
        char_grids = [char_to_grid(c) for c in row_chars]
        # 输出 6 行像素 + 1 行标签
        for py in range(6):
            line_parts = []
            for g in char_grids:
                line_parts.append(g[py])
            lines.append(" ".join(line_parts))
        # 标签行: 字符名居中在4列中
        label_parts = []
        for j, c in enumerate(row_chars):
            idx = grid_row * COLS + j
            name = chr(idx + 32) if 32 <= idx + 32 <= 126 else '?'
            # 特殊字符转义
            if name == ' ':
                name = 'SP'
            elif name == '\\':
                name = '\\\\'
            elif name == "'":
                name = "''"
            # 4字符宽居中
            label_parts.append(f"{name:^4}")
        lines.append(" ".join(label_parts))
        lines.append("")  # 行间空行
    
    with open("font_preview.txt", "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print(f"生成 font_preview.txt")

if __name__ == "__main__":
    main()
