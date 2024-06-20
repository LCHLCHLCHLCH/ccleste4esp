from PIL import Image
import os

def crop_and_save_image(image_path, output_folder):
    # 打开图片并获取尺寸
    img = Image.open(image_path)
    width, height = img.size

    # 裁剪图片的上半部分
    upper_half = img.crop((0, 0, width, height // 2))

    # 将上半部分的图片切成16x8个8x8的小方块
    block_size = 8
    for row in range(8):
        for col in range(16):
            block = upper_half.crop((col * block_size, row * block_size, (col + 1) * block_size, (row + 1) * block_size))

            # 保存这些小方块到文件夹中，并按照指定的命名规则命名
            block.save(os.path.join(output_folder, f"{row * 16 + col}.png"))

# 使用示例

# 当前文件夹的路径
directory_path = os.path.dirname(os.path.abspath(__file__))


image_path = directory_path+"/sprite.png"
output_folder = directory_path+"/sprite_output"
crop_and_save_image(image_path, output_folder)
