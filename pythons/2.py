import zipfile
zip_path = 'download.zip'
# 指定解压目录
extract_to_path = '.'

# 使用with语句确保文件正确关闭
with zipfile.ZipFile(zip_path, 'r') as zip_ref:
    # 解压所有文件到指定目录
    zip_ref.extractall(extract_to_path)