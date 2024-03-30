from PIL import Image
import os

def bmptoico(f,b):
    # 打开png文件
    with Image.open(f) as im:
        # 把PNG转换为ICO
        im.save(b,sizes=[im.size])

# 获取目录下文件名
files = os.listdir()
# 给图标文件单独创建一个icon目录
if not os.path.exists('bmps'):
    os.mkdir('bmps')
for inName in files:
# 分离文件名与扩展名
    tmp = os.path.splitext(inName)
    if tmp[1] == '.ico':
        outName = '.\\bmps\\'+tmp[0] + '.bmp'
        bmptoico(inName,outName)
