from PIL import Image
import os


color_list = [
    0x0,
    0x1D2B53,
    0x7E2553,
    0x8751,
    0xAB5236,
    0x5F574F,
    0xC2C3C7,
    0xFFF1E8,
    0xFF004D,
    0xFFA300,
    0xFFEC27,
    0xE436,
    0x29ADFF,
    0x83769C,
    0xFF77A8,
    0xFFCCAA,
]


# # 当前文件夹的路径
# directory_path = os.path.dirname(os.path.abspath(__file__))

# image = Image.open(directory_path + "/sprite.png")

# c_array = "const uint16_t sprite_sheet[64][128] = {"
# for i in range(64):
#     c_array += "{"
#     for j in range(128):
#         r,g,b = image.getpixel((j,i))
#         color = ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)
#         index = color_list.index(color)
#         c_array += f"{index}, "
#     c_array = c_array[:-2] + "},"
# c_array = c_array[:-1] + "};"

# with open(directory_path + "/sprite.h", "w") as f:
#     f.write("#include<stdint.h>\n")
#     f.write("#ifndef SPRITE_H\n")
#     f.write("#define SPRITE_H\n")
#     f.write(c_array)
#     f.write("\n#endif")

a = 0
for color in color_list:
    r = (color >> 16) & 0xFF
    g = (color >> 8) & 0xFF
    b = color & 0xFF
    rgb = ((r >> 5) & 0x1F) << 11 | ((g >> 5) & 0x3F) << 5 | ((b >> 5) & 0x1F)
    print(f"{a}:{rgb}")
    a = a + 1
