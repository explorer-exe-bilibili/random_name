/*
 * UI编辑模式布局持久化测试
 * 
 * 此示例用于测试按钮布局的保存和加载功能
 * 确保重启后自定义布局能够正确加载
 */

#include "core/screen/base.h"
#include "core/baseItem/Button.h"
#include "core/Config.h"
#include "core/log.h"

using namespace screen;
using namespace core;

class TestScreen : public Screen {
private:
    std::shared_ptr<Button> testButton;
    
public:
    TestScreen() : Screen(ScreenID::MainMenu) {}
    
    void init() override {
        Screen::init();
        
        // 创建测试按钮
        testButton = std::make_shared<Button>("测试按钮", FontID::Default, Region(0.1f, 0.1f, 0.3f, 0.2f));
        testButton->SetClickFunc([this]() {
            Log << "测试按钮被点击" << op::endl;
        });
        
        buttons.push_back(testButton);
        
        Log << "测试屏幕初始化完成" << op::endl;
    }
    
    void enter(int param) override {
        Log << "进入测试屏幕" << op::endl;
        
        // 输出当前按钮位置
        if (testButton) {
            Region region = testButton->GetRegion();
            Log << "按钮当前位置: (" 
                << region.getOriginX() << ", " << region.getOriginY() 
                << ", " << region.getOriginXEnd() << ", " << region.getOriginYEnd() 
                << ")" << op::endl;
        }
    }
    
    void exit() override {
        Log << "退出测试屏幕" << op::endl;
    }
    
    // 手动测试布局保存功能
    void TestSaveLayout() {
        if (testButton) {
            // 模拟移动按钮到新位置
            Region newRegion(0.5f, 0.5f, 0.7f, 0.6f);
            testButton->SetRegion(newRegion);
            
            Log << "按钮移动到新位置: (" 
                << newRegion.getOriginX() << ", " << newRegion.getOriginY() 
                << ", " << newRegion.getOriginXEnd() << ", " << newRegion.getOriginYEnd() 
                << ")" << op::endl;
            
            // 保存布局
            SaveButtonLayout();
            Log << "布局已保存" << op::endl;
        }
    }
    
    // 手动测试布局加载功能
    void TestLoadLayout() {
        Log << "加载布局前的按钮位置:" << op::endl;
        if (testButton) {
            Region region = testButton->GetRegion();
            Log << "  (" << region.getOriginX() << ", " << region.getOriginY() 
                << ", " << region.getOriginXEnd() << ", " << region.getOriginYEnd() 
                << ")" << op::endl;
        }
        
        LoadButtonLayout();
        
        Log << "加载布局后的按钮位置:" << op::endl;
        if (testButton) {
            Region region = testButton->GetRegion();
            Log << "  (" << region.getOriginX() << ", " << region.getOriginY() 
                << ", " << region.getOriginXEnd() << ", " << region.getOriginYEnd() 
                << ")" << op::endl;
        }
    }
    
    // 手动测试布局重置功能
    void TestResetLayout() {
        Log << "重置布局" << op::endl;
        ResetButtonLayout();
        
        if (testButton) {
            Region region = testButton->GetRegion();
            Log << "重置后的按钮位置: (" 
                << region.getOriginX() << ", " << region.getOriginY() 
                << ", " << region.getOriginXEnd() << ", " << region.getOriginYEnd() 
                << ")" << op::endl;
        }
    }
    
    // 测试是否有自定义布局
    void TestHasCustomLayout() {
        bool hasCustom = HasCustomLayout();
        Log << "是否有自定义布局: " << (hasCustom ? "是" : "否") << op::endl;
    }
};

/*
 * 测试步骤说明：
 * 
 * 1. 运行程序，观察初始按钮位置
 * 2. 按F2进入编辑模式
 * 3. 拖拽按钮到新位置
 * 4. 按F2退出编辑模式
 * 5. 重启程序，验证按钮是否在移动后的位置
 * 6. 按F3重置布局，验证按钮是否回到初始位置
 * 
 * 预期结果：
 * - 步骤5中，按钮应该立即显示在移动后的位置（无需进入编辑模式）
 * - 步骤6中，按钮应该回到初始的默认位置
 * 
 * 如果测试失败：
 * - 检查Screen::SwitchToScreen()中是否调用了LoadButtonLayout()
 * - 检查Screen::updateTransition()中是否调用了LoadButtonLayout()
 * - 检查HasCustomLayout()方法是否正确检测自定义布局的存在
 */

// 示例测试代码（不会实际运行，仅作说明）
void RunLayoutPersistenceTest() {
    TestScreen screen;
    screen.init();
    
    // 测试初始状态
    screen.TestHasCustomLayout();
    
    // 模拟保存自定义布局
    screen.TestSaveLayout();
    
    // 测试是否检测到自定义布局
    screen.TestHasCustomLayout();
    
    // 模拟屏幕切换（测试加载功能）
    screen.TestLoadLayout();
    
    // 测试重置功能
    screen.TestResetLayout();
    screen.TestHasCustomLayout();
}
