from PIL import Image
import os


def closest_color(hex_color):
    color_list = [
        "000000",
        "1D2B53",
        "7E2553",
        "008751",
        "AB5236",
        "5F574F",
        "C2C3C7",
        "FFF1E8",
        "FF004D",
        "FFA300",
        "FFEC27",
        "00E436",
        "29ADFF",
        "83769C",
        "FF77A8",
        "FFCCAA",
    ]
    min_distance = float("inf")
    closest_index = -1
    for i, color in enumerate(color_list):
        distance = (
            sum((int(a, 16) - int(b, 16)) ** 2 for a, b in zip(hex_color, color)) ** 0.5
        )
        if distance < min_distance:
            min_distance = distance
            closest_index = i
    return closest_index


# 当前文件夹的路径
directory_path = os.path.dirname(os.path.abspath(__file__))

image = Image.open(directory_path + "/sprite.png")




width, height = image.size
half_height = height // 2
top_half = image.crop((0, 0, width, half_height))
top_half = top_half.convert("RGB")

# c_array = "const uint16_t sprite_sheet[] = {"
# for i in range(64):
#     for j in range(128):
#         r, b, g = image.getpixel((j, i))
#         color = ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)
#         index = closest_color(str(color))
#         c_array += f"{index}, "
# c_array = c_array[:-2] + "};"

c_array = "const uint16_t sprite_sheet[64][128] = {"
for i in range(64):
    c_array += "{"
    for j in range(128):
        r, b, g = image.getpixel((j, i))
        color = ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)
        index = closest_color(str(color))
        c_array += f"{index}, "
    c_array = c_array[:-2] + "},"
c_array = c_array[:-1] + "};"

with open(directory_path + "/sprite.h", "w") as f:
    f.write("#include<stdint.h>\n")
    f.write("#ifndef SPRITE_H\n")
    f.write("#define SPRITE_H\n")
    f.write(c_array)
    f.write("\n#endif")
