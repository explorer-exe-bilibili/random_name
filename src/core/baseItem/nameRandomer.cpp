#include "core/nameRandomer.h"
#include <fstream>
#include <vector>
#include <iostream>

namespace core
{
std::shared_ptr<NameRandomer> NameRandomer::instance = nullptr;

NameRandomer::NameRandomer() : index(0) {
    // 初始化随机数分布
    dist = std::uniform_int_distribution<int>(0, 999);
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
    
    // 这里应该添加文件内容的解析逻辑
    // 具体实现取决于文件的格式

    return true;
}

NameEntry NameRandomer::GetNameEntry() {
    // 默认实现，返回一个基本的 NameEntry
    return NameEntry("测试武器", 4, NameType::sword);
}

} // namespace core
