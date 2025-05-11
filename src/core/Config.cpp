#include "core/Config.h"
#include "core/log.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#pragma execution_character_set("utf-8")

namespace core {

// 初始化静态成员
Config* Config::instance = nullptr;

// 配置项构造函数
ConfigItem::ConfigItem(const std::string& name, const std::string& value)
    : name(name), value(value) {}

// 配置类构造函数
Config::Config() : configFilePath("files/config.txt") {
    // 默认配置文件路径
}

// 获取单例实例
Config* Config::getInstance() {
    if (instance == nullptr) {
        Log << Level::Info << "Creating Config instance" << op::endl;
        instance = new Config();
    }
    return instance;
}

// 析构函数
Config::~Config() {
    Log << Level::Info << "Destroying Config instance" << op::endl;
    cleanup();
}

// 初始化配置系统
bool Config::init(const std::string& configPath) {
    // 如果提供了自定义路径，就使用它
    if (!configPath.empty()) {
        configFilePath = configPath;
    }
    Log << Level::Info << "Config::init() using custom config path: " << configFilePath << op::endl;

    // 确保配置文件目录存在
    std::filesystem::path path(configFilePath);
    if (!std::filesystem::exists(path.parent_path())) {
        Log << Level::Warn << "Config File is Missing try Creating config directory: " << configFilePath << op::endl;
        try {
            std::filesystem::create_directories(path.parent_path());
        } catch (const std::exception& e) {
            Log << Level::Error << "Error creating directory: " << e.what() << op::endl;
            return false;
        }
    }

    // 读取配置文件
    return readFromFile();
}

// 添加配置项
void Config::add(const std::string& name, const std::string& value) {
    configItems[name] = value;
}

void Config::add(const std::string& name, int value) {
    configItems[name] = std::to_string(value);
}

void Config::add(const std::string& name, double value) {
    configItems[name] = std::to_string(value);
}

void Config::add(const std::string& name, bool value) {
    configItems[name] = value ? "1" : "0";
}

// 获取配置值
std::string Config::get(const std::string& name, const std::string& defaultValue) {
    auto it = configItems.find(name);
    if (it != configItems.end()) {
        return it->second;
    }
    return defaultValue;
}

std::string Config::getPath(const std::string& name, const std::string& defaultValue) {
    std::string path = get(name, defaultValue);
    
    // 处理绝对路径和相对路径
    std::filesystem::path fsPath(path);
    if (fsPath.is_relative()) {
        // 获取可执行文件所在目录
        std::filesystem::path exePath = std::filesystem::current_path();
        fsPath = exePath / fsPath;
    }
    
    return fsPath.string();
}

int Config::getInt(const std::string& name, int defaultValue) {
    std::string value = get(name, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return std::stoi(value);
    } catch (const std::exception& e) {
        std::cerr << "Error converting value to int: " << e.what() << std::endl;
        return defaultValue;
    }
}

double Config::getDouble(const std::string& name, double defaultValue) {
    std::string value = get(name, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return std::stod(value);
    } catch (const std::exception& e) {
        std::cerr << "Error converting value to double: " << e.what() << std::endl;
        return defaultValue;
    }
}

bool Config::getBool(const std::string& name, bool defaultValue) {
    std::string value = get(name, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    // 将值统一为小写
    std::transform(value.begin(), value.end(), value.begin(), 
                  [](unsigned char c){ return std::tolower(c); });
    
    // 判断真值条件
    if (value == "1" || value == "true" || value == "yes" || value == "on") {
        return true;
    }
    
    // 判断假值条件
    if (value == "0" || value == "false" || value == "no" || value == "off") {
        return false;
    }
    
    return defaultValue;
}

// 设置配置值 - 修复递归问题
void Config::set(const std::string& name, const std::string& value) {
    configItems[name] = value;
    saveToFile(); // 自动保存
}

void Config::set(const std::string& name, int value) {
    // 修复：直接设置值，避免调用其他重载
    configItems[name] = std::to_string(value);
    saveToFile(); // 自动保存
}

void Config::set(const std::string& name, double value) {
    // 修复：直接设置值，避免调用其他重载
    configItems[name] = std::to_string(value);
    saveToFile(); // 自动保存
}

void Config::set(const std::string& name, bool value) {
    // 修复：直接设置值，避免调用其他重载
    configItems[name] = value ? "1" : "0";
    saveToFile(); // 自动保存
}

void Config::setifno(const std::string &name, const std::string &value)
{
    if (configItems.find(name) == configItems.end()) {
        configItems[name] = value;
        saveToFile(); // 自动保存
    }
}

void Config::setifno(const std::string &name, int value)
{
    if (configItems.find(name) == configItems.end()) {
        configItems[name] = std::to_string(value);
        saveToFile(); // 自动保存
    }
}

void Config::setifno(const std::string &name, double value)
{
    if (configItems.find(name) == configItems.end()) {
        configItems[name] = std::to_string(value);
        saveToFile(); // 自动保存
    }
}

void Config::setifno(const std::string &name, bool value)
{
    if (configItems.find(name) == configItems.end()) {
        configItems[name] = value ? "1" : "0";
        saveToFile(); // 自动保存
    }
}


// 删除配置项
void Config::remove(const std::string& name) {
    auto it = configItems.find(name);
    if (it != configItems.end()) {
        configItems.erase(it);
        saveToFile(); // 自动保存
    }
}

// 读取配置文件
bool Config::readFromFile() {
    std::ifstream file(configFilePath);
    Log<<Level::Info << "Config::readFromFile() using config path: " << configFilePath << op::endl;
    if (!file.is_open()) {
        // 如果文件不存在，创建一个空的配置文件
        std::ofstream newFile(configFilePath);
        if (newFile.is_open()) {
            newFile.close();
            return true;
        }
        Log << Level::Error << "Error opening config file for reading: " << configFilePath << op::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // 跳过空行和注释行
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // 解析键值对
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string name = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            
            // 去除首尾空格
            name.erase(0, name.find_first_not_of(" \t"));
            name.erase(name.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            add(name, value);
        }
        Log << Level::Info << "Config::readFromFile() loaded: " << line << op::endl;
    }
    Log << Level::Info << "Config::readFromFile() finished loading config file" << op::endl;
    file.close();
    return true;
}

// 保存配置文件
bool Config::saveToFile() {
    std::ofstream file(configFilePath);
    Log << Level::Info << "Config::saveToFile() using config path: " << configFilePath << op::endl;
    if (!file.is_open()) {
        Log << Level::Error << "Error opening config file for writing: " << configFilePath << op::endl;
        return false;
    }
    
    // 写入头部信息
    file << "# Configuration file\n";
    file << "# Generated on " << __DATE__ << " " << __TIME__ << "\n\n";
    
    // 写入配置项
    for (const auto& item : configItems) {
        file << item.first << "=" << item.second << "\n";
    }

    Log << Level::Info << "Config::saveToFile() finished writing config file" << op::endl;
    file.close();
    return true;
}

// 切换布尔值
bool Config::toggleBool(const std::string& name) {
    bool currentValue = getBool(name, false);
    bool newValue = !currentValue;
    
    // 修复：直接设置值，避免调用导致递归
    configItems[name] = newValue ? "1" : "0";
    saveToFile();
    
    return newValue;
}

// 设置屏幕尺寸
void Config::setScreenSize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
}

// 设置窗口尺寸
void Config::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

// 获取屏幕比例尺寸
int Config::getScreenScaled(const std::string& name, int defaultValue) {
    std::string value = get(name, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        double doubleValue = std::stod(value);
        // 如果值大于1，则视为绝对像素值
        if (doubleValue > 1.0) {
            return static_cast<int>(doubleValue);
        }
        
        // 否则视为比例值
        if (name.find("width") != std::string::npos || name.back() == 'x' || name.back() == 'X') {
            return static_cast<int>(doubleValue * screenWidth);
        } 
        else if (name.find("height") != std::string::npos || name.back() == 'y' || name.back() == 'Y') {
            return static_cast<int>(doubleValue * screenHeight);
        }
    } catch (const std::exception& e) {
        Log << Level::Error << "Error converting scaled value: " << e.what();
    }
    
    return defaultValue;
}

// 获取窗口比例尺寸
int Config::getWindowScaled(const std::string& name, int defaultValue) {
    std::string value = get(name, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        double doubleValue = std::stod(value);
        // 如果值大于1，则视为绝对像素值
        if (doubleValue > 1.0) {
            return static_cast<int>(doubleValue);
        }
        
        // 否则视为比例值
        if (name.find("width") != std::string::npos || name.back() == 'x' || name.back() == 'X') {
            return static_cast<int>(doubleValue * windowWidth);
        } 
        else if (name.find("height") != std::string::npos || name.back() == 'y' || name.back() == 'Y') {
            return static_cast<int>(doubleValue * windowHeight);
        }
    } catch (const std::exception& e) {
        Log << Level::Error << "Error converting scaled value: " << e.what();
    }
    
    return defaultValue;
}

// 清理资源
void Config::cleanup() {
    configItems.clear();
    Log << Level::Info << "Configuration system cleaned up" << op::endl;
}

// 打印所有配置项（调试用）
void Config::printAllConfigItems() {
    Log << Level::Info << "Configuration items:" << op::endl;

    for (const auto& item : configItems) {
        Log << Level::Info << item.first << " --- " << item.second << op::endl;
    }

    Log << Level::Info << "Total items: " << configItems.size() << op::endl;
}

} // namespace core