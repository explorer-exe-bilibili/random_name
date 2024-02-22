import PythonMagick
from PIL import Image

img = PythonMagick.Image(r'1.bmp')
# 指定图标的尺寸
bmp = Image.open('1.bmp')
width, height = bmp.size
img.sample(str(width)+'x'+str(height))
# 保存图标文件
img.write('1.ico')
