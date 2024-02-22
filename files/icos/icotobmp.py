from PIL import Image

def convert_bmp_to_ico(bmp_file, ico_file):
    # 加载bmp图像
    with Image.open(bmp_file) as image:
        # 获取原始图像的尺寸
        original_size = image.size

        # 保存为bmp文件，使用原始图像的尺寸
        image.save(ico_file, format='BMP', sizes=[original_size])

# 使用示例
convert_bmp_to_ico('1.ico', '1.bmp')
