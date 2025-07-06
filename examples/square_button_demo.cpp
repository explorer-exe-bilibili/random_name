/*
 * Region 1:1比例功能使用指南
 * 
 * 演示如何使用Region的1:1比例功能和编辑模式兼容性
 */

#include "core/baseItem/Button.h"
#include "core/screen/base.h"

using namespace core;
using namespace screen;

class SquareButtonDemo : public Screen {
private:
    std::shared_ptr<Button> squareIcon;
    std::shared_ptr<Button> rectangleButton;
    
public:
    SquareButtonDemo() : Screen(ScreenID::MainMenu) {}
    
    void init() override {
        Screen::init();
        
        // 方法1：使用传统方式创建1:1比例按钮（yend <= 0）
        Region squareRegion1(0.1f, 0.1f, 0.2f, -1.0f); // yend = -1 表示1:1比例
        squareIcon = std::make_shared<Button>("图标", FontID::Default, squareRegion1);
        squareIcon->SetClickFunc([]() {
            Log << "正方形图标被点击" << op::endl;
        });
        buttons.push_back(squareIcon);
        
        // 方法2：使用新的显式方式创建1:1比例按钮
        Region squareRegion2(0.3f, 0.1f, 0.4f, 0.2f); // 先创建普通区域
        auto squareButton = std::make_shared<Button>("正方形按钮", FontID::Default, squareRegion2);
        squareButton->GetRegion().setAspectRatio1to1(true); // 显式启用1:1比例
        squareButton->SetClickFunc([]() {
            Log << "1:1比例按钮被点击" << op::endl;
        });
        buttons.push_back(squareButton);
        
        // 方法3：创建普通矩形按钮（对比用）
        Region rectRegion(0.5f, 0.1f, 0.8f, 0.2f);
        rectangleButton = std::make_shared<Button>("矩形按钮", FontID::Default, rectRegion);
        rectangleButton->SetClickFunc([]() {
            Log << "矩形按钮被点击" << op::endl;
        });
        buttons.push_back(rectangleButton);
        
        Log << "按钮初始化完成:" << op::endl;
        Log << "- 传统1:1按钮: 宽=" << squareIcon->GetRegion().getWidth() 
            << ", 高=" << squareIcon->GetRegion().getHeight() 
            << ", 1:1=" << (squareIcon->GetRegion().isAspectRatio1to1() ? "是" : "否") << op::endl;
        Log << "- 新式1:1按钮: 宽=" << squareButton->GetRegion().getWidth() 
            << ", 高=" << squareButton->GetRegion().getHeight() 
            << ", 1:1=" << (squareButton->GetRegion().isAspectRatio1to1() ? "是" : "否") << op::endl;
        Log << "- 矩形按钮: 宽=" << rectangleButton->GetRegion().getWidth() 
            << ", 高=" << rectangleButton->GetRegion().getHeight() 
            << ", 1:1=" << (rectangleButton->GetRegion().isAspectRatio1to1() ? "是" : "否") << op::endl;
    }
    
    void enter(int param) override {
        Log << "进入正方形按钮演示屏幕" << op::endl;
        Log << "按F2进入编辑模式，测试1:1比例按钮的编辑功能" << op::endl;
        Log << "编辑时，1:1比例按钮会保持正方形" << op::endl;
    }
};

/*
 * 使用说明：
 * 
 * 1. 创建1:1比例按钮的方法：
 *    a) 传统方式：Region(x, y, xend, yend <= 0)
 *    b) 新方式：region.setAspectRatio1to1(true)
 * 
 * 2. 编辑模式兼容性：
 *    - 进入编辑模式时，1:1比例按钮显示为正方形
 *    - 拖拽移动时，保持正方形
 *    - 调整大小时，自动维持1:1比例
 *    - 最小尺寸限制确保不会变得太小
 * 
 * 3. 检查1:1比例：
 *    - 使用region.isAspectRatio1to1()检查
 *    - 1:1模式下getWidth() == getHeight()
 * 
 * 4. 切换模式：
 *    - setAspectRatio1to1(true)启用1:1比例
 *    - setAspectRatio1to1(false)禁用1:1比例
 *    - setyend(正数)会自动禁用1:1比例
 * 
 * 测试步骤：
 * 1. 运行程序，观察三种按钮的初始形状
 * 2. 按F2进入编辑模式
 * 3. 拖拽1:1比例按钮，观察是否保持正方形
 * 4. 调整1:1比例按钮大小，观察是否保持正方形
 * 5. 对比矩形按钮的自由调整
 * 6. 按F2退出编辑模式，保存更改
 */
