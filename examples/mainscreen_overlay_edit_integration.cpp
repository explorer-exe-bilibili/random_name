/*
 * MainScreen overlays编辑功能集成示例
 * 
 * 此文件展示如何修改现有的MainScreen类，
 * 为overlays容器中的按钮添加编辑功能
 */

// 在MainScreen.cpp中需要进行的修改示例：

#include "core/screen/mainScreen.h"
#include "core/log.h"

// 修改1：在init()方法末尾添加overlays注册
void MainScreen::init() {
    // ...existing code... (原有的初始化代码)
    
    // 为overlays容器中的按钮注册编辑功能
    Log << "注册overlay按钮到编辑系统" << op::endl;
    for (const auto& overlay : overlays) {
        if (overlay) {
            RegisterEditableButton(overlay);
        }
    }
    
    Log << "MainScreen overlay编辑功能已启用，共注册了 " 
        << overlays.size() << " 个overlay按钮" << op::endl;
}

// 修改2：在析构函数中清理（如果需要）
MainScreen::~MainScreen() {
    // ...existing code...
    
    // 清理注册的编辑按钮（可选，基类析构时会自动清理）
    ClearEditableButtons();
}

// 修改3：重写SaveButtonLayout方法以包含overlays
void MainScreen::SaveButtonLayout() {
    // 调用基类方法保存主要按钮
    Screen::SaveButtonLayout();
    
    // 保存overlays的布局
    core::Config* config = core::Config::getInstance();
    std::string screenPrefix = "screen_" + std::to_string(static_cast<int>(getID()));
    
    for (size_t i = 0; i < overlays.size(); ++i) {
        if (overlays[i]) {
            std::string overlayName = screenPrefix + "_overlay_" + std::to_string(i);
            // 使用Button的内置保存方法
            core::Region region = overlays[i]->GetRegion();
            config->set(overlayName, region);
        }
    }
    
    config->saveToFile();
    Log << "MainScreen overlay布局已保存" << op::endl;
}

// 修改4：添加overlays的布局加载方法
void MainScreen::LoadOverlayLayout() {
    core::Config* config = core::Config::getInstance();
    std::string screenPrefix = "screen_" + std::to_string(static_cast<int>(getID()));
    
    for (size_t i = 0; i < overlays.size(); ++i) {
        if (overlays[i]) {
            std::string overlayName = screenPrefix + "_overlay_" + std::to_string(i);
            core::Region defaultRegion = overlays[i]->GetRegion();
            core::Region region = config->getRegion(overlayName, 
                                                   core::RegionName::NONE, 
                                                   defaultRegion);
            
            // 如果加载的区域与默认区域不同，说明有自定义布局
            if (region.getOriginX() != defaultRegion.getOriginX() || 
                region.getOriginY() != defaultRegion.getOriginY() ||
                region.getOriginXEnd() != defaultRegion.getOriginXEnd() || 
                region.getOriginYEnd() != defaultRegion.getOriginYEnd()) {
                overlays[i]->SetRegion(region);
            }
        }
    }
    
    Log << "MainScreen overlay布局已加载" << op::endl;
}

// 修改5：在enter()方法中加载布局
void MainScreen::enter(int param) {
    // ...existing code...
    
    // 加载overlay的自定义布局
    LoadOverlayLayout();
    
    Log << "进入主屏幕，overlay编辑功能可用" << op::endl;
}

/*
 * MainScreen.h中需要添加的声明：
 * 
 * private:
 *     void LoadOverlayLayout();  // 加载overlay布局的方法
 * 
 * 或者重写基类的LoadButtonLayout方法来包含overlays
 */

// 可选：重写LoadButtonLayout来统一处理所有按钮布局
void MainScreen::LoadButtonLayout() {
    // 调用基类方法
    Screen::LoadButtonLayout();
    
    // 加载overlays布局
    LoadOverlayLayout();
}

/*
 * 实际修改步骤：
 * 
 * 1. 在MainScreen::init()末尾添加RegisterEditableButton调用
 * 2. 重写SaveButtonLayout()方法包含overlays保存
 * 3. 添加LoadOverlayLayout()方法或重写LoadButtonLayout()
 * 4. 在enter()方法中调用布局加载
 * 5. 在析构函数中调用ClearEditableButtons()（可选）
 * 
 * 测试验证：
 * 1. 运行程序，进入主屏幕
 * 2. 按F2进入编辑模式
 * 3. 验证overlay按钮是否显示编辑框
 * 4. 拖拽或调整overlay按钮
 * 5. 按F2退出编辑模式
 * 6. 重启程序，验证overlay位置是否保持
 */
