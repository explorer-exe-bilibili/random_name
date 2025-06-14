#include "core/nameRandomer.h"
#include "core/Config.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <cctype>
#include "core/log.h"

namespace core
{
std::vector<std::shared_ptr<NameRandomer>> NameRandomer::instances;

NameRandomer* NameRandomer::getInstance(int mode)  {
        if(mode>=instances.size()){
            instances.resize(4);
            instances[0]=createInstance(0);
            instances[1]=createInstance(1);
            instances[2]=createInstance(2);
            instances[3]=createInstance(3);
            instances[0]->setFile(Config::getInstance()->getPath(NAMES1, "files/name/names.txt"));
            instances[1]->setFile(Config::getInstance()->getPath(NAMES2, "files/name/names.txt"));
            instances[2]->setFile(Config::getInstance()->getPath(NAMES3, "files/name/names.txt"));
            instances[3]->setFile(Config::getInstance()->getPath(NAMES4, "files/name/names.txt"));
            return getInstance(mode);
        }
        std::shared_ptr<NameRandomer> instance = instances[mode];
        if(instance==nullptr){
            instance=createInstance(mode);
            instances[mode]=instance;
        }
        return instance.get();
    }


NameRandomer::NameRandomer(int index) : index(index) {
    // 初始化随机数分布
    dist = std::uniform_int_distribution<int>(0, 999);
}

std::shared_ptr<NameRandomer> NameRandomer::createInstance(int index) {
    return std::shared_ptr<NameRandomer>(new NameRandomer(index));
}

NameRandomer::~NameRandomer() {
    // 清理资源
}

bool NameRandomer::setFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filePath << std::endl;
        return false;
    }
    
    entries.clear();
    std::string line;
      while (std::getline(file, line)) {
        // 跳过空行
        if (line.empty()) continue;
        
        // 解析每行格式：名字+数字（星数+元素类型+武器类型）
        NameEntry entry;
        
        // 从后往前找数字部分
        std::string name = line;
        std::string numbers = "";
          // 找到最后连续的数字部分
        int i = line.length() - 1;
        while (i >= 0 && std::isdigit(static_cast<unsigned char>(line[i]))) {
            numbers = line[i] + numbers;
            i--;
        }
        
        // 名字部分是除去数字后的部分
        if (i >= 0) {
            name = line.substr(0, i + 1);
        } else {
            name = line; // 如果全是数字，保留原始字符串作为名字
        }
        
        entry.name = name;
        
        // 解析数字部分，默认值
        entry.star = 3;
        entry.type = NameType::sword;
        entry.mode = index; // 设置模式为当前索引
        // 根据数字串的长度解析不同属性
        if (!numbers.empty()) {
            if (numbers.length() >= 1) {
                // 第一个数字是星数
                entry.star = numbers[0] - '0';
                if (entry.star < 3) entry.star = 3;
                else if(entry.star > 6) entry.star = 6; // 限制星数在3到6之间
            }
            
            if (numbers.length() >= 2) {
                // 第二个数字是武器类型
                int weaponType = numbers[1] - '0';
                switch (weaponType) {
                    case 0: entry.type = NameType::bow; break;
                    case 1: entry.type = NameType::sword; break;
                    case 2: entry.type = NameType::catalyst; break;
                    case 3: entry.type = NameType::claymore; break;
                    case 4: entry.type = NameType::polearm; break;
                    default: entry.type = NameType::Unknow; break;
                }
            }
            
            if (numbers.length() >= 3) {
                // 第三个数字是元素类型（暂时忽略，因为NameEntry没有这个字段）
                // int elementType = numbers[1] - '0';
            }
        }
        
        entries.push_back(entry);
    }
    
    // 更新随机数分布范围
    if (!entries.empty()) {
        dist = std::uniform_int_distribution<int>(0, entries.size() - 1);
    }
    
    file.close();
    return !entries.empty();
}

bool NameRandomer::addName(NameEntry entry) {
    // 检查条目是否有效
    if (entry.name.empty()) {
        std::cerr << "无效的条目: " << entry.name << std::endl;
        return false;
    }
    entry.star = std::max(3, std::min(6, entry.star)); // 确保星数在3到6之间
    entry.mode = index; // 设置模式为当前索引
    
    // 添加条目到列表中
    entries.push_back(entry);
    std::filebuf fileBuf;
    std::string filePath = Config::getInstance()->
        getPath(("names"+std::to_string(index+1)), "files/name/"+std::to_string(index+1)+".txt");
    if (!fileBuf.open(filePath, std::ios::out | std::ios::app)) {
        std::cerr << "无法打开文件: " << filePath << std::endl;
        return false;
    }
    std::ostream out(&fileBuf);
    // 写入条目到文件
    out << entry.name;
    out << entry.star; // 写入星数
    out << int(entry.type); // 写入武器类型
    out << "\n"; // 换行符
    fileBuf.close();
    Log << "添加条目: " << entry.name << " 星数: " << entry.star << " 类型: " << static_cast<int>(entry.type) << op::endl;
    // 更新随机数分布范围
    dist = std::uniform_int_distribution<int>(0, entries.size() - 1);
    
    return true;
}

NameEntry NameRandomer::GetNameEntry() {
    // 如果没有加载任何条目，返回默认值
    if (entries.empty()) {
        return NameEntry("默认", 3, NameType::Unknow,index);
    }
    
    // 随机选择一个条目
    int randomIndex = dist(rng)% entries.size();
    if (randomIndex < 0 || randomIndex >= entries.size()) {
        randomIndex = 0; // 确保索引在有效范围内
    }
    return entries[randomIndex];
}

} // namespace core
