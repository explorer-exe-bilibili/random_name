import os
import glob
from PIL import Image

def convert_webp_to_bmp(directory, output_directory):
    # 确保输出目录存在
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # 获取目录下所有的WebP文件
    webp_files = glob.glob(os.path.join(directory, '*.webp'))

    # 遍历所有WebP文件
    for webp_file in webp_files:
        # 获取文件名和扩展名
        file_name, file_ext = os.path.splitext(os.path.basename(webp_file))
        # 创建新的BMP文件名
        bmp_file = os.path.join(output_directory, file_name + '.bmp')

        # 打开WebP文件
        with Image.open(webp_file) as img:
            # 保存为BMP格式
            img.save(bmp_file, 'BMP')

# 使用示例
input_directory = 'C:\\Users\\Administrator\\source\\repos\\ramdon name\\files\\imgs'  # 替换为你的输入目录路径
output_directory = 'C:\\Users\\Administrator\\source\\repos\\ramdon name\\files\\imgs'  # 替换为你想要保存BMP文件的目录路径

convert_webp_to_bmp(input_directory, output_directory)
