#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "configItem.h"
#include "baseItem/Base.h"

namespace core {

// 定义配置项结构
class ConfigItem {
public:
    ConfigItem() = default;
    ConfigItem(const std::string& name, const std::string& value);
    
    std::string name;
    std::string value;
};

// 配置管理类
class Config {
private:
    // 单例实例
    static Config* instance;
    
    // 配置项存储
    std::unordered_map<std::string, std::string> configItems;
    
    // 配置文件路径
    std::string configFilePath;
    
    // 屏幕和窗口尺寸缓存
    int screenWidth = 0;
    int screenHeight = 0;

    // 构造函数私有化
    Config();
    
    // 使用set直接设置值，无需使用add手动添加
    void add(const std::string& name, const std::string& value);
    void add(const std::string& name, int value);
    void add(const std::string& name, unsigned int value);
    void add(const std::string& name, double value);
    void add(const std::string& name, bool value);
    
    
public:
    // 禁止复制构造和赋值操作符
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // 获取单例实例
    static Config* getInstance();
    
    // 析构函数
    ~Config();

    // 初始化配置系统
    bool init(const std::string& configPath = "");

    // 获取配置值
    std::string get(const std::string& name, const std::string& defaultValue = "");
    std::string getPath(const std::string& name, const std::string& defaultValue = "");
    Region getRegion(const std::string& name, const Region& defaultValue = {});
    int getInt(const std::string& name, int defaultValue = 0);
    unsigned int getUInt(const std::string& name, unsigned int defaultValue = 0);
    long long getLong(const std::string& name, long long defaultValue = 0);
    double getDouble(const std::string& name, double defaultValue = 0.0);
    bool getBool(const std::string& name, bool defaultValue = false);
    float getRatioedInt(const std::string& name, float defaultValue = 0.0f);
    
    // 替换配置值
    void set(const std::string& name, const std::string& value);
    void set(const std::string& name, int value);
    void set(const std::string& name, float value, float isRatioed= false);
    void set(const std::string& name, unsigned int value);
    void set(const std::string& name, double value);
    void set(const std::string& name, bool value);
    void set(const std::string& name, const Region& region);
    void set(const RegionName category, const std::string& name, const Region& region);
    
    // 设置默认值（如果不存在则添加）
    void setifno(const std::string& name, const std::string& value);
    void setifno(const std::string& name, const char* value);
    void setifno(const std::string& name, int value);
    void setifno(const std::string& name, unsigned int value);
    void setifno(const std::string& name, double value);
    void setifno(const std::string& name, bool value);
    void setifno(const std::string& name, const Region& region);
    void setifno(const RegionName category, const std::string& name, const Region& region);
    // 删除配置项
    void remove(const std::string& name);
    
    // 读写配置文件
    bool readFromFile();
    bool saveToFile();
    bool createEmptyConfigFile();
    bool tryRepairJsonFile(std::string& content);
    bool tryLegacyFileFormat();
    
    // 切换布尔值
    bool toggleBool(const std::string& name);
    
    // 尺寸相关功能
    void setScreenSize(int width, int height);
    
    // JSON相关功能
    nlohmann::json getAsJson(const std::string& name, const nlohmann::json& defaultValue = nlohmann::json());
    void setJson(const std::string& name, const nlohmann::json& value);
    
    // 清理资源
    void cleanup();
    
    void setCurrentRegionName(const RegionName& regionName);

    // 区域验证和修正
    bool validateRegion(const Region& region);
    Region normalizeRegion(const Region& region);
};

} // namespace core