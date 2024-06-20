# current_folder = os.path.dirname(os.path.abspath(__file__))

# print(current_folder)

# img = Image.open(current_folder+"/screenshot.png")

# img.resize((128,128))

# f = open(current_folder+"/pic.c", "w")
# f.write("hello")
# f.close()

from PIL import Image
import numpy as np
import os


def image_to_rgb565(image_path):
	# 打开图片并转换为RGB模式
	image = Image.open(image_path).convert("RGB")
	image = image.resize((128, 128))
	
	width, height = image.size

	# 提取每个像素的颜色值并转为RGB565
	rgb565_data = []
	for y in range(height):
		row = []
		for x in range(width):
			r, g, b = image.getpixel((x, y))
			
			# 稍有偏差
			# r = (r >> 3) & 0x1F  # Convert R to 5 bits
			# g = (g >> 2) & 0x3F  # Convert G to 6 bits
			# b = (b >> 3) & 0x1F  # Convert B to 5 bits
			# rgb = ((r >> 2) & 0x3F) << 10 | ((g >> 3) & 0x1F) << 5 | ((b >> 3) & 0x1F)
			
			# 稍有偏差
			# rgb = ((r >> 5) & 0x1F) << 11 | ((g >> 5) & 0x3F) << 5 | ((b >> 5) & 0x1F)
			rgb = ((r >> 5) & 0x1F) << 11 | ((g >> 5) & 0x3F) << 5 | ((b >> 5) & 0x1F)
			
			# 偏差很大
			# r_5bit = (r >> 3) & 0x1F  # 保留红色高5位
			# g_6bit = (g >> 2) & 0x3F  # 保留绿色高6位
			# b_5bit = (b >> 3) & 0x1F  # 保留蓝色高5位
			# rgb = (r_5bit << 11) | (g_6bit << 5) | b_5bit
			
			# RBG565
			# r5 = (r >> 3) & 0x1F    # 取红色的前5位
			# b6 = (b >> 2) & 0x3F    # 取蓝色的前6位
			# g5 = (g >> 3) & 0x1F    # 取绿色的前5位
			# rbg565 = (r5 << 11) | (b6 << 5) | g5
			
			#RBG556
			# r5 = (r >> 3) & 0x1F    # 取红色的前5位
			# b5 = (b >> 3) & 0x1F    # 取蓝色的前5位
			# g6 = (g >> 2) & 0x3F    # 取绿色的前6位
			# rbg556 = (r5 << 11) | (b5 << 6) | g6
			
			# RBG555
			# r5 = (r >> 3) & 0x1F    # 取红色的前5位
			# b5 = (b >> 3) & 0x1F    # 取蓝色的前5位
			# g5 = (g >> 3) & 0x1F    # 取绿色的前5位
			# rbg555 = (r5 << 10) | (b5 << 5) | g5
			
			#rgb565
			# r0 = r*32//256
			# g0 = g*64//256
			# b0 = b*32//256
			# r0 = r0*(2**11)
			# g0 = g0*(2**5)
			# rbg565 = r0|g0|b0
			
			row.append(rgb)
		rgb565_data.append(row)

	return rgb565_data


def write_c_header(rgb565_data, output_file):
	with open(output_file, "w") as f:
		f.write("#ifndef RGB565_DATA_H\n")
		f.write("#define RGB565_DATA_H\n")
		f.write("#include <stdint.h>\n")
		f.write("#define WIDTH 128\n")
		f.write("#define HEIGHT 128\n")
		f.write("uint16_t rgb565_data[HEIGHT][WIDTH] = {")

		for row in rgb565_data:
			f.write("    {")
			for value in row[:-1]:
				f.write("0x{:04X}, ".format(value))
			f.write("0x{:04X}}},".format(row[-1]))

		f.write("};\n")
		f.write("#endif // RGB565_DATA_H")


if __name__ == "__main__":
	current_folder = os.path.dirname(os.path.abspath(__file__))
	image_path = current_folder + "/screenshot.png"
	print(image_path)
	# output_file = current_folder + "/pic.h"
	output_file = "/home/lch/project/hardware_SPI/main/Drivers/LCD/pic.h"

	rgb565_data = image_to_rgb565(image_path)
	write_c_header(rgb565_data, output_file)
	print("已将图片转换为RGB565数据并写入头文件：", output_file)
