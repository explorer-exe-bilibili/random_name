import requests
import re
from json import dumps,loads

# 假设这些是你的Alist V3 API的URL和认证信息
alist_api_url = "http://52.184.81.18:35244/"
token = ""  # 请替换为你的API token
def from_str(x) -> str:
    assert isinstance(x, str)
    return x
    
    
def parse_version(filename):
    # 使用正则表达式找到版本号
    match = version_pattern.search(filename)
    if match:
        # 提取版本号的数字部分
        version_parts = match.group(2).split('.')
        # 转换为整数列表
        for num in version_parts:
            if num!='' and num!='zip':
                version_numbers=int(num)
                version_numbers = [num]
        # 提取预发布版本或构建元数据（如果有）
        prerelease = match.group(3)
        return version_numbers
    else:
        return None

def get_token():
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
    print(token)

# 列出文件目录的API路径
list_files_api = f"{alist_api_url}api/fs/list"
# 获取文件信息的API路径
get_file_info_api = f"{alist_api_url}api/fs/get"
get_token()
# 头部信息，包含认证token
headers = {
   'Authorization': token,
   'User-Agent': 'Apifox/1.0.0 (https://apifox.com)',
   'Content-Type': 'application/json'
}

# 请求参数
params = {
    "path": "",  # 假设我们要列出的目录是 /t
    "password": "",
    "page": 1,
    "per_page": 0,
    "refresh": False
}

# 获取文件列表
response = requests.post(list_files_api, json=params, headers=headers)
files_data = response.json()

# 检查响应状态
if files_data["code"] == 200:
    files = files_data["data"].get("content", [])
    
    # 正则表达式匹配版本号格式的文件
    version_pattern = re.compile(r'^(v|)?(\d+\.){1,2}(\d+)(\-[a-zA-Z0-9]+(\.[a-zA-Z0-9]+)*|\d+)?(\.[a-zA-Z0-9]+)*$')
    version_files = [file for file in files if version_pattern.match(file["name"])]
    
    # 如果找到了版本号格式的文件
    if version_files:
        # 找出最大版本号的文件
        parsed_versions = [parse_version(file["name"]) for file in version_files]
        max_version_file = max(version_files, key=lambda x: list(map(int, x["name"].split('.'))))
        
        # 获取最大版本号文件的详细信息
        file_info_params = {
            "path": max_version_file["path"],
            "password": "",
            "page": 1,
            "per_page": 0,
            "refresh": False
        }
        file_info_response = requests.post(get_file_info_api, json=file_info_params, headers=headers)
        file_info = file_info_response.json()
        
        # 检查响应状态
        if file_info["code"] == 200:
            raw_url = file_info["data"].get("raw_url")
            print(f"最大版本号文件的raw_url是: {raw_url}")
        else:
            print("获取文件信息失败。")
    else:
        print("没有找到符合版本号格式的文件。")
        print(files)
else:
    print("获取文件列表失败。")
    print(response)
    print(files_data["code"])