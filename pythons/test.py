import requests
from json import dumps,loads
from time import sleep
from typing import Any
from tqdm import tqdm
import win32api
from pathlib import Path
import zipfile
import os

token=''
file_name=''

# 定义文件路径
file_path = Path('version')

# 读取文件内容
content = file_path.read_text(encoding='ansi')

def from_str(x: Any) -> str:
    assert isinstance(x, str)
    return x
replacements = {
    "一": ":",
    "二": "/",
    "三": "?",
    "四": "<",
    "五": ">",
    "六": "|",
    "七": "*",
    "八": "<",
    "九": "\\",
}
def trim_string_until_char(s, char):
    # 找到特殊字符的索引
    index = s.find(char)
    # 如果找到了特殊字符
    if index != -1:
        # 裁剪掉特殊字符之前的所有内容
        return s[index + 1:]  # 加1是因为我们要从特殊字符之后开始裁剪
    else:
        # 如果没有找到特殊字符，返回原始字符串或None
        return s  # 或者 return None，取决于您的需求

def getverson():
    url = "http://52.184.81.18:35244/api/auth/login" 

    payload = dumps({
       "username": "apiuser",
       "password": "12345678"
    })
    headers = {
       'User-Agent': 'Apifox/1.0.0 (https://apifox.com)',
       'Content-Type': 'application/json'
    }

    response = requests.request("POST", url, headers=headers, data=payload)
    json_response = loads(response.text)  # 解析JSON字符串为字典
    obj = json_response.get("data")  # 获取包含token的数据部分
    token = from_str(obj.get("token"))  # 获取token
    url = "http://52.184.81.18:35244/api/fs/list" 
    payload = dumps({
       "path": "/",
       "password": "",
       "page": 1,
       "per_page": 0,
       "refresh": False
    })
    headers = {
       'Authorization': token,
       'User-Agent': 'Apifox/1.0.0 (https://apifox.com)',
       'Content-Type': 'application/json'
    }

    response = requests.request("POST", url, headers=headers, data=payload)
    # 解析JSON字符串为字典
    json_response = loads(response.text)

    # 检查 "data" 和 "content" 是否存在
    if "data" in json_response and "content" in json_response["data"] and isinstance(json_response["data"]["content"], list):
        # 处理 "content" 列表
        contents = json_response["data"]["content"]
        for content in contents:
            # 这里可以访问每个文件的信息，例如文件名
            file_name = content.get("name")
    else:
        print("Error: 'content' key is missing or not a list.")
        
    # 遍历替换映射字典并替换所有匹配项
    for old, new in replacements.items():
        file_name = file_name.replace(old, str(new))  # 使用str确保非字符串替换也能工作

    return file_name

def get_string_until_char(s, char):
    # 找到特殊字符的索引
    index = s.find(char)
    # 如果找到了特殊字符
    if index != -1:
        # 返回特殊字符之前的内容
        return s[:index]
    else:
        # 如果没有找到特殊字符，返回整个字符串
        return s


def download(url):

    # 发送GET请求
    response = requests.get(url, stream=True)

    # 检查请求是否成功
    if response.status_code == 200:
        # 打开一个文件以写入二进制内容
            # 获取文件总大小
        file_size = int(response.headers.get('content-length', 0))
        
        # 创建进度条
        progress_bar = tqdm(total=file_size, unit='B', unit_scale=True, desc='Downloading', leave=True)
        
        with open('download.zip', 'wb') as file:
            # 以块的形式写入文件以节省内存
            for chunk in response.iter_content(chunk_size=8192):
                progress_bar.update(len(chunk))  # 更新进度条
                file.write(chunk)
        print("文件下载成功，保存为 'downloaded.zip'")
    else:
        print(f"下载失败，状态码：{response.status_code}")
        
# 替换映射字典
version=''
versions=getverson()
version=versions
number=get_string_until_char(version,"版")
version=trim_string_until_char(version,"本")
if content<number:
    download(version)
        
    zip_path = 'download.zip'
    # 指定解压目录
    extract_to_path = '.'

    # 使用with语句确保文件正确关闭
    with zipfile.ZipFile(zip_path, 'r') as zip_ref:
        # 解压所有文件到指定目录
        zip_ref.extractall(extract_to_path)
else:
    print('已经是最新版本',content)
    