/*
 * 为非buttons容器中的Button对象添加编辑功能的示例
 * 
 * 此示例展示如何使用新的RegisterEditableButton功能
 * 为MainScreen中的overlays按钮添加编辑能力
 */

#include "core/screen/mainScreen.h"
#include "core/log.h"

using namespace screen;
using namespace core;

/*
 * 使用说明：
 * 
 * 1. 问题描述：
 *    某些Button对象不在主要的buttons容器中，如：
 *    - MainScreen的overlays容器
 *    - 静态的exitButton
 *    - 其他特殊按钮
 * 
 * 2. 解决方案：
 *    基类Screen新增了additionalEditableButtons容器和管理方法：
 *    - RegisterEditableButton(): 注册额外的可编辑按钮
 *    - UnregisterEditableButton(): 取消注册
 *    - ClearEditableButtons(): 清空所有额外按钮
 * 
 * 3. 使用步骤：
 *    a) 在屏幕初始化时调用RegisterEditableButton()
 *    b) 编辑模式会自动应用到注册的按钮
 *    c) 在屏幕销毁前调用UnregisterEditableButton()
 */

// 示例：修改MainScreen来支持overlays的编辑
class EnhancedMainScreen : public MainScreen {
public:
    EnhancedMainScreen() : MainScreen() {}
    
    void init() override {
        // 调用父类初始化
        MainScreen::init();
        
        // 为overlays容器中的按钮注册编辑功能
        Log << "注册overlays按钮到编辑系统" << op::endl;
        for (const auto& overlay : overlays) {
            if (overlay) {
                RegisterEditableButton(overlay);
                Log << "注册overlay按钮到编辑模式" << op::endl;
            }
        }
        
        Log << "MainScreen overlay编辑功能已启用" << op::endl;
    }
    
    ~EnhancedMainScreen() {
        // 清理注册的按钮
        Log << "清理注册的编辑按钮" << op::endl;
        ClearEditableButtons();
    }
    
    void SaveButtonLayout() override {
        // 调用父类保存方法
        MainScreen::SaveButtonLayout();
        
        // 额外保存overlays的布局
        core::Config* config = core::Config::getInstance();
        std::string screenPrefix = "screen_" + std::to_string(static_cast<int>(getID()));
        
        for (size_t i = 0; i < overlays.size(); ++i) {
            if (overlays[i]) {
                std::string overlayName = screenPrefix + "_overlay_" + std::to_string(i);
                overlays[i]->SaveRegionToConfig(overlayName);
            }
        }
        
        config->saveToFile();
        Log << "Overlay布局已保存" << op::endl;
    }
};

/*
 * 通用示例：为任何屏幕添加额外按钮的编辑功能
 */
class CustomScreen : public Screen {
private:
    std::shared_ptr<core::Button> specialButton1;
    std::shared_ptr<core::Button> specialButton2;
    std::vector<std::shared_ptr<core::Button>> customButtons;
    
public:
    CustomScreen() : Screen(ScreenID::Settings) {}
    
    void init() override {
        Screen::init();
        
        // 创建特殊按钮
        specialButton1 = std::make_shared<core::Button>("特殊按钮1", FontID::Default, 
                                                       core::Region(0.1f, 0.1f, 0.3f, 0.2f));
        specialButton2 = std::make_shared<core::Button>("特殊按钮2", FontID::Default, 
                                                       core::Region(0.1f, 0.3f, 0.3f, 0.4f));
        
        // 创建自定义按钮容器
        for (int i = 0; i < 3; ++i) {
            auto btn = std::make_shared<core::Button>("自定义" + std::to_string(i), FontID::Default,
                                                     core::Region(0.5f + i * 0.1f, 0.1f, 0.6f + i * 0.1f, 0.2f));
            customButtons.push_back(btn);
        }
        
        // 注册所有特殊按钮到编辑系统
        RegisterEditableButton(specialButton1);
        RegisterEditableButton(specialButton2);
        
        for (const auto& btn : customButtons) {
            RegisterEditableButton(btn);
        }
        
        Log << "自定义屏幕的额外按钮已注册到编辑系统" << op::endl;
    }
    
    void Draw() override {
        Screen::Draw(); // 绘制基本内容
        
        // 绘制特殊按钮
        if (specialButton1) specialButton1->Draw();
        if (specialButton2) specialButton2->Draw();
        
        // 绘制自定义按钮
        for (const auto& btn : customButtons) {
            if (btn) btn->Draw();
        }
    }
    
    bool Click(int x, int y) override {
        // 先检查基类的点击处理
        if (Screen::Click(x, y)) return true;
        
        core::Point point(x, y, false);
        
        // 处理特殊按钮的点击
        if (specialButton1 && specialButton1->OnClick(point)) return true;
        if (specialButton2 && specialButton2->OnClick(point)) return true;
        
        // 处理自定义按钮的点击
        for (const auto& btn : customButtons) {
            if (btn && btn->OnClick(point)) return true;
        }
        
        return false;
    }
    
    void enter(int param) override {
        Log << "进入自定义屏幕，按F2可编辑所有按钮（包括特殊按钮）" << op::endl;
    }
    
    ~CustomScreen() {
        // 清理注册的按钮
        ClearEditableButtons();
    }
};

/*
 * 使用总结：
 * 
 * 1. 基类改进：
 *    - 添加了additionalEditableButtons容器
 *    - 所有编辑方法自动处理额外的按钮
 *    - 提供了注册/取消注册的管理接口
 * 
 * 2. 子类使用：
 *    - 在init()中调用RegisterEditableButton()注册按钮
 *    - 在析构函数中调用ClearEditableButtons()清理
 *    - 重写SaveButtonLayout()以保存额外按钮的布局
 * 
 * 3. 自动功能：
 *    - 编辑模式自动应用到注册的按钮
 *    - 拖拽、调整大小等功能自动可用
 *    - 编辑覆盖层自动绘制
 *    - 布局保存回调自动设置
 * 
 * 4. 测试方法：
 *    - 运行程序，进入相应屏幕
 *    - 按F2进入编辑模式
 *    - 验证额外的按钮是否显示编辑框和手柄
 *    - 测试拖拽和调整大小功能
 */
