/**
 * @file region_config_example.cpp
 * @brief Region配置功能使用示例
 * 
 * 这个文件展示了如何使用Config类的Region相关功能
 */

#include "core/Config.h"
#include "core/configItem.h"
#include "core/log.h"
#include "core/baseItem/Base.h"

using namespace core;

void regionConfigExample() {
    Log << Level::Info << "=== Region Configuration Example ===" << op::endl;
    
    // 获取Config实例
    Config* config = Config::getInstance();
    
    // 1. 基本Region操作示例
    Log << Level::Info << "1. Basic Region Operations" << op::endl;
    
    // 创建一个按钮区域 (屏幕坐标比例: 左上角10%,80% 到 右下角30%,90%)
    Region buttonRegion(0.1, 0.8, 0.3, 0.9, true);
    
    // 设置Region配置
    config->setRegion(UI_REGION_BUTTON_AREA, buttonRegion);
    
    // 获取Region配置
    Region loadedButtonRegion = config->getRegion(UI_REGION_BUTTON_AREA, Region(0, 0, 1, 1));
    Log << Level::Info << "Button Region: (" << loadedButtonRegion.getOriginX() << "," 
        << loadedButtonRegion.getOriginY() << ") to (" << loadedButtonRegion.getOriginXEnd() 
        << "," << loadedButtonRegion.getOriginYEnd() << ")" << op::endl;
    
    // 2. 设置默认Region（如果不存在才设置）
    Log << Level::Info << "2. Setting Default Regions" << op::endl;
    
    config->setifnoRegion(UI_REGION_MAIN_MENU, Region(0.0, 0.0, 1.0, 1.0, true));
    config->setifnoRegion(UI_REGION_SETTING_PANEL, Region(0.2, 0.2, 0.8, 0.8, true));
    config->setifnoRegion(UI_REGION_VIDEO_PLAYER, Region(0.1, 0.1, 0.9, 0.7, true));
    
    // 3. 命名区域管理示例
    Log << Level::Info << "3. Named Region Management" << op::endl;
    
    // 为不同类型的按钮设置区域
    config->setNamedRegion("buttons", "start", Region(0.4, 0.5, 0.6, 0.6, true));
    config->setNamedRegion("buttons", "exit", Region(0.4, 0.7, 0.6, 0.8, true));
    config->setNamedRegion("buttons", "settings", Region(0.85, 0.05, 0.95, 0.15, true));
    
    // 为不同的面板设置区域
    config->setNamedRegion("panels", "inventory", Region(0.0, 0.0, 0.3, 1.0, true));
    config->setNamedRegion("panels", "character", Region(0.7, 0.0, 1.0, 1.0, true));
    
    // 获取命名区域
    Region startButtonRegion = config->getNamedRegion("buttons", "start", Region(0.4, 0.5, 0.6, 0.6, true));
    Log << Level::Info << "Start Button Region: (" << startButtonRegion.getOriginX() << "," 
        << startButtonRegion.getOriginY() << ") to (" << startButtonRegion.getOriginXEnd() 
        << "," << startButtonRegion.getOriginYEnd() << ")" << op::endl;
    
    // 4. Region列表操作示例
    Log << Level::Info << "4. Region List Operations" << op::endl;
    
    // 创建一组相关的区域
    std::vector<Region> overlayRegions = {
        Region(0.0, 0.0, 1.0, 0.1, true),  // 顶部栏
        Region(0.0, 0.9, 1.0, 1.0, true),  // 底部栏
        Region(0.0, 0.1, 0.1, 0.9, true),  // 左侧栏
        Region(0.9, 0.1, 1.0, 0.9, true)   // 右侧栏
    };
    
    config->setRegionList("overlay_regions", overlayRegions);
    
    // 获取Region列表
    std::vector<Region> loadedOverlays = config->getRegionList("overlay_regions");
    Log << Level::Info << "Loaded " << loadedOverlays.size() << " overlay regions" << op::endl;
    
    // 5. Region预设管理示例
    Log << Level::Info << "5. Region Preset Management" << op::endl;
    
    // 定义默认布局预设
    std::unordered_map<std::string, Region> defaultLayout = {
        {"main_menu", Region(0.1, 0.1, 0.9, 0.9, true)},
        {"button_area", Region(0.2, 0.8, 0.8, 0.95, true)},
        {"video_player", Region(0.1, 0.1, 0.9, 0.7, true)},
        {"setting_panel", Region(0.2, 0.2, 0.8, 0.8, true)}
    };
    config->defineRegionPreset(REGION_PRESET_DEFAULT, defaultLayout);
    
    // 定义全屏布局预设
    std::unordered_map<std::string, Region> fullscreenLayout = {
        {"main_menu", Region(0.0, 0.0, 1.0, 1.0, true)},
        {"button_area", Region(0.1, 0.9, 0.9, 1.0, true)},
        {"video_player", Region(0.0, 0.0, 1.0, 0.9, true)},
        {"setting_panel", Region(0.0, 0.0, 1.0, 1.0, true)}
    };
    config->defineRegionPreset(REGION_PRESET_FULLSCREEN, fullscreenLayout);
    
    // 定义紧凑布局预设
    std::unordered_map<std::string, Region> compactLayout = {
        {"main_menu", Region(0.05, 0.05, 0.95, 0.95, true)},
        {"button_area", Region(0.1, 0.85, 0.9, 0.95, true)},
        {"video_player", Region(0.05, 0.05, 0.95, 0.8, true)},
        {"setting_panel", Region(0.1, 0.1, 0.9, 0.9, true)}
    };
    config->defineRegionPreset(REGION_PRESET_COMPACT, compactLayout);
    
    // 获取可用的预设列表
    auto availablePresets = config->getAvailableRegionPresets();
    Log << Level::Info << "Available region presets: ";
    for (const auto& preset : availablePresets) {
        Log << Level::Info << preset << " ";
    }
    Log << Level::Info << op::endl;
    
    // 应用预设
    Log << Level::Info << "Applying default preset..." << op::endl;
    if (config->applyRegionPreset(REGION_PRESET_DEFAULT)) {
        Log << Level::Info << "Default preset applied successfully" << op::endl;
    } else {
        Log << Level::Error << "Failed to apply default preset" << op::endl;
    }
    
    // 6. Region验证示例
    Log << Level::Info << "6. Region Validation" << op::endl;
    
    // 测试有效区域
    Region validRegion(0.1, 0.1, 0.9, 0.9, true);
    Log << Level::Info << "Valid region test: " << (config->validateRegion(validRegion) ? "PASS" : "FAIL") << op::endl;
    
    // 测试无效区域（负坐标）
    Region invalidRegion1(-0.1, 0.1, 0.9, 0.9, true);
    Log << Level::Info << "Invalid region test (negative coords): " << (config->validateRegion(invalidRegion1) ? "FAIL" : "PASS") << op::endl;
    
    // 测试无效区域（end < start）
    Region invalidRegion2(0.9, 0.9, 0.1, 0.1, true);
    Log << Level::Info << "Invalid region test (end < start): " << (config->validateRegion(invalidRegion2) ? "FAIL" : "PASS") << op::endl;
    
    // 测试区域标准化
    Region unnormalizedRegion(-0.1, -0.1, 1.2, 1.2, true);
    Region normalizedRegion = config->normalizeRegion(unnormalizedRegion);
    Log << Level::Info << "Normalized region: (" << normalizedRegion.getOriginX() << "," 
        << normalizedRegion.getOriginY() << ") to (" << normalizedRegion.getOriginXEnd() 
        << "," << normalizedRegion.getOriginYEnd() << ")" << op::endl;
    
    Log << Level::Info << "=== Region Configuration Example Complete ===" << op::endl;
}

// 实际使用场景示例
void practicalUsageExample() {
    Log << Level::Info << "=== Practical Usage Example ===" << op::endl;
    
    Config* config = Config::getInstance();
    
    // 游戏启动时初始化默认区域
    config->setifnoRegion(UI_REGION_MAIN_MENU, Region(0.1, 0.1, 0.9, 0.9, true));
    config->setifnoRegion(UI_REGION_NAME_DISPLAY, Region(0.2, 0.3, 0.8, 0.7, true));
    config->setifnoRegion(UI_REGION_CONTROL_PANEL, Region(0.1, 0.8, 0.9, 0.95, true));
    
    // 根据屏幕分辨率调整布局
    // 假设这是在窗口大小改变时调用的函数
    auto adjustLayoutForResolution = [&](int width, int height) {
        if (width < 1280 || height < 720) {
            // 小屏幕使用紧凑布局
            config->applyRegionPreset(REGION_PRESET_COMPACT);
            Log << Level::Info << "Applied compact layout for small screen" << op::endl;
        } else if (width >= 1920 && height >= 1080) {
            // 大屏幕使用默认布局
            config->applyRegionPreset(REGION_PRESET_DEFAULT);
            Log << Level::Info << "Applied default layout for large screen" << op::endl;
        }
    };
    
    // 模拟不同分辨率
    adjustLayoutForResolution(1024, 768);  // 小屏幕
    adjustLayoutForResolution(1920, 1080); // 大屏幕
    
    Log << Level::Info << "=== Practical Usage Example Complete ===" << op::endl;
}

// 如果这个文件被直接编译执行
#ifdef REGION_CONFIG_EXAMPLE_STANDALONE
int main() {
    // 初始化配置系统
    Config* config = Config::getInstance();
    config->init("config_example.json");
    
    // 运行示例
    regionConfigExample();
    practicalUsageExample();
    
    return 0;
}
#endif
