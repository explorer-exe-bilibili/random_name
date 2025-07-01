#include "core/Config.h"
#include "core/log.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <regex>
#include <nlohmann/json.hpp>


namespace core {

// 初始化静态成员
Config* Config::instance = nullptr;

// 配置项构造函数
ConfigItem::ConfigItem(const std::string& name, const std::string& value)
    : name(name), value(value) {}

// 配置类构造函数
Config::Config() : configFilePath("files/config.json") {
    // 默认配置文件路径改为.json格式
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

    // 检查是否有旧版本的配置文件(.txt格式)
    std::filesystem::path jsonPath(configFilePath);
    std::filesystem::path txtPath = jsonPath;
    if (jsonPath.extension() == ".json") {
        txtPath.replace_extension(".txt");
    } else {
        // 如果提供的不是.json，确保我们有一个.json的路径
        jsonPath = configFilePath;
        if (jsonPath.extension() != ".json") {
            jsonPath.replace_extension(".json");
            configFilePath = jsonPath.string();
        }
    }

    // 如果JSON文件不存在但TXT文件存在，从TXT迁移配置
    if (!std::filesystem::exists(jsonPath) && std::filesystem::exists(txtPath)) {
        Log << Level::Info << "Found legacy config file, will migrate to JSON format: " << txtPath << op::endl;
        configFilePath = txtPath.string();  // 先设置为旧文件路径
        if (readFromFile()) {  // 读取旧文件
            configFilePath = jsonPath.string();  // 然后设置为新文件路径
            saveToFile();  // 保存为JSON格式
            Log << Level::Info << "Successfully migrated config from " << txtPath << " to " << jsonPath << op::endl;
        }
        configFilePath = jsonPath.string();  // 确保后续使用JSON文件路径
    }

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

void Config::add(const std::string& name, unsigned int value) {
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
    // 添加输入参数验证
    if (name.empty()) {
        Log << Level::Warn << "Config::get() called with empty name" << op::endl;
        return defaultValue;
    }
    
    try {
        // 验证name字符串的内存有效性
        const char* nameData = name.c_str();
        if (nameData == nullptr) {
            Log << Level::Error << "Config::get() - name string data is null" << op::endl;
            return defaultValue;
        }
        
        auto it = configItems.find(name);
        if (it != configItems.end()) {
            // 验证返回值的有效性
            if (it->second.length() > 10000) { // 合理的长度检查
                Log << Level::Warn << "Config::get() - value too long for key: " << name << op::endl;
                return defaultValue;
            }
            return it->second;
        }
    }
    catch (const std::exception& e) {
        Log << Level::Error << "Config::get() - Exception: " << e.what() << " for key: " << name << op::endl;
        return defaultValue;
    }
    catch (...) {
        Log << Level::Error << "Config::get() - Unknown exception for key: " << name << op::endl;
        return defaultValue;
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
    if (core::isFileExists(fsPath.string())) {
        return fsPath.string();
    }
    else{
        std::filesystem::path defaultFsPath(defaultValue);
        if (defaultFsPath.is_relative()) {
            // 获取可执行文件所在目录
            std::filesystem::path exePath = std::filesystem::current_path();
            defaultFsPath = exePath / defaultFsPath;
        }
        return defaultFsPath.string();
    }
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

unsigned int Config::getUInt(const std::string& name, unsigned int defaultValue) {
    std::string value = get(name, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return static_cast<unsigned int>(std::stoul(value));
    } catch (const std::exception& e) {
        std::cerr << "Error converting value to unsigned int: " << e.what() << std::endl;
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

// 获取配置值为JSON对象
nlohmann::json Config::getAsJson(const std::string& name, const nlohmann::json& defaultValue) {
    std::string value = get(name, "");
    if (value.empty()) {
        return defaultValue;
    }
    
    try {
        return nlohmann::json::parse(value);
    } catch (const nlohmann::json::parse_error& e) {
        Log << Level::Error << "Error parsing JSON value for key " << name << ": " << e.what() << op::endl;
        return defaultValue;
    }
}

// 设置配置值为JSON对象
void Config::setJson(const std::string& name, const nlohmann::json& value) {
    try {
        std::string jsonStr = value.dump();
        configItems[name] = jsonStr;
        saveToFile(); // 自动保存
    } catch (const std::exception& e) {
        Log << Level::Error << "Error setting JSON value for key " << name << ": " << e.what() << op::endl;
    }
}

// 设置配置值 - 修复递归问题
void Config::set(const std::string& name, const std::string& value) {
    configItems[name] = value;
}

void Config::set(const std::string& name, int value) {
    // 修复：直接设置值，避免调用其他重载
    configItems[name] = std::to_string(value);
}

void Config::set(const std::string& name, unsigned int value) {
    configItems[name] = std::to_string(value);
}

void Config::set(const std::string& name, double value) {
    // 修复：直接设置值，避免调用其他重载
    configItems[name] = std::to_string(value);
}

void Config::set(const std::string& name, bool value) {
    // 修复：直接设置值，避免调用其他重载
    configItems[name] = value ? "1" : "0";
}

void Config::setifno(const std::string &name, const std::string &value)
{
    if (configItems.find(name) == configItems.end()) {
        Log<<Level::Info << "Config::setifno() setting default string value for: " << name << " to " << value << op::endl;
        configItems[name] = value;
    }
}

void Config::setifno(const std::string &name, const char* value)
{
    if (configItems.find(name) == configItems.end()) {
        Log<<Level::Info << "Config::setifno() setting default string value for: " << name << " to " << value << op::endl;
        configItems[name] = value;
    }
}

void Config::setifno(const std::string &name, int value)
{
    if (configItems.find(name) == configItems.end()) {
        Log<<Level::Info << "Config::setifno() setting default int value for: " << name << " to " << value << op::endl;
        configItems[name] = std::to_string(value);
    }
}

void Config::setifno(const std::string &name, unsigned int value)
{
    if (configItems.find(name) == configItems.end()) {
        Log<<Level::Info << "Config::setifno() setting default unsigned int value for: " << name << " to " << value << op::endl;
        configItems[name] = std::to_string(value);
    }
}

void Config::setifno(const std::string &name, double value)
{
    if (configItems.find(name) == configItems.end()) {
        Log<<Level::Info << "Config::setifno() setting default double value for: " << name << " to " << value << op::endl;
        configItems[name] = std::to_string(value);
        saveToFile(); // 自动保存
    }
}

void Config::setifno(const std::string &name, bool value)
{
    if (configItems.find(name) == configItems.end()) {
        Log<<Level::Info << "Config::setifno() setting default bool value for: " << name << " to " << value << op::endl;
        configItems[name] = value ? "1" : "0";
        saveToFile(); // 自动保存
    }
}

// 设置屏幕尺寸
void Config::setScreenSize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
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
    std::filesystem::path configPath(configFilePath);
    Log << Level::Info << "Config::readFromFile() using config path: " << configFilePath << op::endl;
    
    // 检查备份文件
    std::filesystem::path backupPath = configPath;
    backupPath.replace_extension(".json.bak");
    bool hasBackup = std::filesystem::exists(backupPath);
    
    // 如果文件不存在，首先尝试从备份恢复
    if (!std::filesystem::exists(configPath) && hasBackup) {
        Log << Level::Info << "Config file not found, but backup exists. Restoring from backup." << op::endl;
        try {
            std::filesystem::copy_file(backupPath, configPath, 
                std::filesystem::copy_options::overwrite_existing);
            Log << Level::Info << "Restored config from backup file" << op::endl;
        } catch (const std::exception& e) {
            Log << Level::Error << "Failed to restore from backup: " << e.what() << op::endl;
            // 继续尝试其他方式
        }
    }
    
    // 打开文件
    std::ifstream file(configFilePath);
    if (!file.is_open()) {
        // 如果文件不存在，创建一个空的配置文件
        Log << Level::Info << "Config file not found, creating a new one: " << configFilePath << op::endl;
        return createEmptyConfigFile();
    }
    
    try {
        // 读取整个文件内容
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // 检查文件是否为空
        if (content.empty()) {
            Log << Level::Warn << "Config file is empty, initializing with default JSON structure" << op::endl;
            return createEmptyConfigFile();
        }
        
        // 尝试解析JSON
        nlohmann::json jsonConfig;
        try {
            jsonConfig = nlohmann::json::parse(content);
        } catch (const nlohmann::json::parse_error& e) {
            Log << Level::Error << "JSON parse error at position " << e.byte << ": " << e.what() << op::endl;
            
            // 尝试修复JSON
            if (!tryRepairJsonFile(content)) {
                // 如果修复失败，尝试使用备份
                if (hasBackup) {
                    Log << Level::Info << "Attempting to restore from backup after parse error" << op::endl;
                    try {
                        std::filesystem::copy_file(backupPath, configPath, 
                            std::filesystem::copy_options::overwrite_existing);
                        Log << Level::Info << "Restored config from backup file" << op::endl;
                        
                        // 重新读取备份文件
                        std::ifstream backupFile(configPath);
                        if (!backupFile.is_open()) {
                            Log << Level::Error << "Failed to open restored config file" << op::endl;
                            return false;
                        }
                        
                        content = std::string((std::istreambuf_iterator<char>(backupFile)), 
                                             std::istreambuf_iterator<char>());
                        backupFile.close();
                        
                        try {
                            jsonConfig = nlohmann::json::parse(content);
                        } catch (const nlohmann::json::parse_error& e) {
                            Log << Level::Error << "Backup file also has JSON errors: " << e.what() << op::endl;
                            return tryLegacyFileFormat();
                        }
                    } catch (const std::exception& e) {
                        Log << Level::Error << "Failed to restore from backup: " << e.what() << op::endl;
                        return tryLegacyFileFormat();
                    }
                } else {
                    return tryLegacyFileFormat();
                }
            } else {
                // 修复成功后重新解析
                try {
                    jsonConfig = nlohmann::json::parse(content);
                    // 保存修复后的文件
                    std::ofstream repaired(configFilePath);
                    if (repaired.is_open()) {
                        repaired << jsonConfig.dump(4);
                        repaired.close();
                        Log << Level::Info << "Successfully repaired and saved JSON file" << op::endl;
                    }
                } catch (const nlohmann::json::parse_error& e) {
                    Log << Level::Error << "Still unable to parse JSON after repair attempt: " << e.what() << op::endl;
                    return tryLegacyFileFormat();
                }
            }
        }
        
        // 清除当前配置项
        configItems.clear();
        
        // 验证JSON结构
        bool isValidFormat = true;
        if (!jsonConfig.is_object()) {
            Log << Level::Error << "Invalid JSON format: root is not an object" << op::endl;
            isValidFormat = false;
        }
        else if (!jsonConfig.contains("config") || !jsonConfig["config"].is_object()) {
            Log << Level::Error << "Invalid JSON format: missing or invalid 'config' section" << op::endl;
            isValidFormat = false;
        }
        
        // 如果验证失败，尝试修复
        if (!isValidFormat) {
            Log << Level::Info << "Attempting to restructure invalid JSON format" << op::endl;
            
            nlohmann::json fixedConfig;
            fixedConfig["_metadata"] = {
                {"generated_on", std::string(__DATE__) + " " + std::string(__TIME__)},
                {"version", "1.0.0"},
                {"recovered", true}
            };
            
            // 如果是普通对象，将其作为config节点
            if (jsonConfig.is_object()) {
                fixedConfig["config"] = jsonConfig;
            } else {
                // 否则创建空的config节点
                fixedConfig["config"] = nlohmann::json::object();
            }
            
            jsonConfig = fixedConfig;
            
            // 保存修复后的结构
            std::ofstream fixedFile(configFilePath);
            if (fixedFile.is_open()) {
                fixedFile << jsonConfig.dump(4);
                fixedFile.close();
                Log << Level::Info << "Successfully restructured and saved JSON file" << op::endl;
            }
        }
        
        // 如果使用的是新的JSON格式（带有config节点）
        nlohmann::json configData = jsonConfig["config"];
        for (auto it = configData.begin(); it != configData.end(); ++it) {
            std::string key = it.key();
            std::string value;
            
            // 处理不同类型的值
            if (it.value().is_string()) {
                value = it.value().get<std::string>();
            } else if (it.value().is_number_integer()) {
                value = std::to_string(it.value().get<int>());
            } else if (it.value().is_number_unsigned()) {
                value = std::to_string(it.value().get<unsigned int>());
            } else if (it.value().is_number_float()) {
                value = std::to_string(it.value().get<double>());
            } else if (it.value().is_boolean()) {
                value = it.value().get<bool>() ? "1" : "0";
            } else {
                // 对于复杂类型，转换为字符串表示
                value = it.value().dump();
            }
            
            configItems[key] = value;
            Log << Level::Info << "Config::readFromFile() loaded: " << key << " = " << value << op::endl;
        }
        
        Log << Level::Info << "Config::readFromFile() finished loading config file" << op::endl;
        return true;
    } catch (const std::exception& e) {
        Log << Level::Error << "Error in readFromFile: " << e.what() << op::endl;
        file.close();
        return false;
    }
}

// 创建空的配置文件
bool Config::createEmptyConfigFile() {
    std::ofstream newFile(configFilePath);
    if (!newFile.is_open()) {
        Log << Level::Error << "Error creating config file: " << configFilePath << op::endl;
        return false;
    }
    
    // 创建一个基本的JSON结构
    nlohmann::json jsonConfig;
    jsonConfig["_metadata"] = {
        {"generated_on", std::string(__DATE__) + " " + std::string(__TIME__)},
        {"version", "1.0.0"}
    };
    jsonConfig["config"] = nlohmann::json::object();
    
    newFile << jsonConfig.dump(4);
    newFile.close();
    Log << Level::Info << "Created new empty config file" << op::endl;
    return true;
}

// 保存配置文件
bool Config::saveToFile() {
    Log << Level::Info << "Config::saveToFile() using config path: " << configFilePath << op::endl;
    
    // 创建备份文件
    std::filesystem::path configPath(configFilePath);
    std::filesystem::path backupPath = configPath;
    backupPath.replace_extension(".json.bak");
    
    // 如果原文件存在，先创建备份
    if (std::filesystem::exists(configPath)) {
        try {
            std::filesystem::copy_file(configPath, backupPath, 
                std::filesystem::copy_options::overwrite_existing);
            Log << Level::Info << "Created backup of config file at: " << backupPath.string() << op::endl;
        } catch (const std::exception& e) {
            Log << Level::Error << "Failed to create backup file: " << e.what() << op::endl;
            // 即使备份失败，也继续尝试保存
        }
    }
    
    // 使用临时文件进行写入，成功后再替换原文件
    std::filesystem::path tempPath = configPath;
    tempPath += ".tmp";
    std::ofstream file(tempPath);
    
    if (!file.is_open()) {
        Log << Level::Error << "Error opening temporary config file for writing: " << tempPath.string() << op::endl;
        return false;
    }
    
    try {
        // 创建JSON对象
        nlohmann::json jsonConfig;
        
        // 添加元数据
        jsonConfig["_metadata"] = {
            {"generated_on", std::string(__DATE__) + " " + std::string(__TIME__)},
            {"version", "1.0.0"}
        };
        
        // 添加配置项
        nlohmann::json configData;
        for (const auto& item : configItems) {
            configData[item.first] = item.second;
        }
        jsonConfig["config"] = configData;
        
        // 格式化JSON并写入文件
        file << jsonConfig.dump(4);  // 使用4个空格缩进，便于人类阅读
        file.flush();  // 确保所有数据都写入磁盘
        
        // 检查写入是否成功
        if (file.fail()) {
            Log << Level::Error << "Failed to write to temporary config file" << op::endl;
            file.close();
            std::filesystem::remove(tempPath);
            return false;
        }
        
        file.close();
        
        // 用临时文件替换原文件
        try {
            std::filesystem::rename(tempPath, configPath);
        } catch (const std::exception& e) {
            Log << Level::Error << "Failed to replace config file with temporary file: " << e.what() << op::endl;
            return false;
        }
        
        Log << Level::Info << "Config::saveToFile() finished writing config file in JSON format" << op::endl;
        return true;
    } catch (const nlohmann::json::exception& e) {
        Log << Level::Error << "JSON error in saveToFile: " << e.what() << op::endl;
        file.close();
        std::filesystem::remove(tempPath);
        return false;
    } catch (const std::exception& e) {
        Log << Level::Error << "Error in saveToFile: " << e.what() << op::endl;
        file.close();
        std::filesystem::remove(tempPath);
        return false;
    }
}

// 清理资源
void Config::cleanup() {
    configItems.clear();
    Log << Level::Info << "Configuration system cleaned up" << op::endl;
}

// 尝试修复损坏的JSON文件
bool Config::tryRepairJsonFile(std::string& content) {
    Log << Level::Info << "Attempting to repair corrupted JSON file" << op::endl;
    
    // 简单修复策略1：去除多余的逗号
    auto fixExtraCommas = [](std::string& str) -> bool {
        bool fixed = false;
        std::regex objectComma(R"(,\s*})");
        std::string fixed_content = std::regex_replace(str, objectComma, "}");
        
        std::regex arrayComma(R"(,\s*\])");
        fixed_content = std::regex_replace(fixed_content, arrayComma, "]");
        
        if (fixed_content != str) {
            str = fixed_content;
            return true;
        }
        return false;
    };
    
    // 简单修复策略2：补全缺失的引号
    auto fixMissingQuotes = [](std::string& str) -> bool {
        bool fixed = false;
        std::regex missingQuoteKey(R"(([{,]\s*)([a-zA-Z0-9_]+)(\s*:))");
        std::string fixed_content = std::regex_replace(str, missingQuoteKey, "$1\"$2\"$3");
        
        if (fixed_content != str) {
            str = fixed_content;
            return true;
        }
        return false;
    };
    
    // 简单修复策略3：尝试修复不完整的JSON文件
    auto fixIncompleteJson = [](std::string& str) -> bool {
        int openBraces = 0, closeBraces = 0;
        int openBrackets = 0, closeBrackets = 0;
        
        for (char c : str) {
            if (c == '{') openBraces++;
            else if (c == '}') closeBraces++;
            else if (c == '[') openBrackets++;
            else if (c == ']') closeBrackets++;
        }
        
        bool fixed = false;
        
        // 补全缺失的右大括号
        while (openBraces > closeBraces) {
            str += "}";
            closeBraces++;
            fixed = true;
        }
        
        // 补全缺失的右中括号
        while (openBrackets > closeBrackets) {
            str += "]";
            closeBrackets++;
            fixed = true;
        }
        
        return fixed;
    };
    
    bool anyFixes = fixExtraCommas(content);
    anyFixes |= fixMissingQuotes(content);
    anyFixes |= fixIncompleteJson(content);
    
    return anyFixes;
}

// 尝试使用旧格式解析文件
bool Config::tryLegacyFileFormat() {
    Log << Level::Info << "Attempting to parse file as legacy key=value format" << op::endl;
    
    std::ifstream file(configFilePath);
    if (!file.is_open()) {
        Log << Level::Error << "Could not open config file for legacy parsing" << op::endl;
        return false;
    }
    
    // 清除当前配置项
    configItems.clear();
    
    std::string line;
    int validEntries = 0;
    
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
            validEntries++;
            Log << Level::Info << "Config::readFromFile() loaded (legacy format): " << name << " = " << value << op::endl;
        }
    }
    
    file.close();
    
    if (validEntries > 0) {
        // 将旧格式转换为新的JSON格式并保存
        Log << Level::Info << "Converting legacy format to JSON format" << op::endl;
        saveToFile();
        return true;
    } else {
        Log << Level::Error << "No valid configuration entries found in legacy format" << op::endl;
        return false;
    }
}

bool Config::toggleBool(const std::string& name) {
    if (configItems.contains(name)) {
        bool currentValue = getBool(name);
        set(name, !currentValue);
        saveToFile();
        return !currentValue;
    }
    return false; // 如果没有找到配置项，返回false

}
}