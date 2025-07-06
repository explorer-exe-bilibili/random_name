/*
 * Region 1:1比例功能测试和说明
 * 
 * 此示例演示Region类的1:1比例功能以及编辑器的兼容性修复
 */

#include "core/baseItem/Base.h"
#include "core/baseItem/Button.h"
#include "core/log.h"

using namespace core;

/*
 * 问题描述：
 * Region类有一个特殊功能：当yend <= 0时，会自动计算1:1比例的高度
 * 这个功能用于创建正方形按钮或保持宽高比的组件
 * 
 * 但是在编辑模式下，编辑器不能正确处理这种特殊情况：
 * 1. 编辑器获取原始坐标时，getOriginYEnd()返回<=0的值
 * 2. 编辑后，直接设置新的yend值，破坏了1:1比例逻辑
 * 3. 导致组件失去1:1比例特性
 */

void TestRegion1to1Feature() {
    Log << "=== Region 1:1比例功能测试 ===" << op::endl;
    
    // 测试1：创建1:1比例的Region（旧方式，通过yend <= 0）
    Log << "测试1：旧方式创建1:1比例Region" << op::endl;
    Region oldStyle(0.1f, 0.1f, 0.3f, -1.0f); // yend = -1表示1:1比例
    Log << "原始坐标: x=" << oldStyle.getOriginX() 
        << ", y=" << oldStyle.getOriginY() 
        << ", xend=" << oldStyle.getOriginXEnd() 
        << ", yend=" << oldStyle.getOriginYEnd() << op::endl;
    Log << "实际渲染坐标: x=" << oldStyle.getx() 
        << ", y=" << oldStyle.gety() 
        << ", xend=" << oldStyle.getxend() 
        << ", yend=" << oldStyle.getyend() << op::endl;
    Log << "宽度: " << oldStyle.getWidth() << ", 高度: " << oldStyle.getHeight() << op::endl;
    Log << "是否1:1比例: " << (oldStyle.isAspectRatio1to1() ? "是" : "否") << op::endl;
    
    // 测试2：创建1:1比例的Region（新方式，显式设置）
    Log << "\n测试2：新方式创建1:1比例Region" << op::endl;
    Region newStyle(0.1f, 0.1f, 0.3f, 0.3f); // 正常创建
    newStyle.setAspectRatio1to1(true); // 显式启用1:1比例
    Log << "原始坐标: x=" << newStyle.getOriginX() 
        << ", y=" << newStyle.getOriginY() 
        << ", xend=" << newStyle.getOriginXEnd() 
        << ", yend=" << newStyle.getOriginYEnd() << op::endl;
    Log << "实际渲染坐标: x=" << newStyle.getx() 
        << ", y=" << newStyle.gety() 
        << ", xend=" << newStyle.getxend() 
        << ", yend=" << newStyle.getyend() << op::endl;
    Log << "宽度: " << newStyle.getWidth() << ", 高度: " << newStyle.getHeight() << op::endl;
    Log << "是否1:1比例: " << (newStyle.isAspectRatio1to1() ? "是" : "否") << op::endl;
    
    // 测试3：普通Region（非1:1比例）
    Log << "\n测试3：普通Region（非1:1比例）" << op::endl;
    Region normal(0.1f, 0.1f, 0.3f, 0.4f);
    Log << "原始坐标: x=" << normal.getOriginX() 
        << ", y=" << normal.getOriginY() 
        << ", xend=" << normal.getOriginXEnd() 
        << ", yend=" << normal.getOriginYEnd() << op::endl;
    Log << "实际渲染坐标: x=" << normal.getx() 
        << ", y=" << normal.gety() 
        << ", xend=" << normal.getxend() 
        << ", yend=" << normal.getyend() << op::endl;
    Log << "宽度: " << normal.getWidth() << ", 高度: " << normal.getHeight() << op::endl;
    Log << "是否1:1比例: " << (normal.isAspectRatio1to1() ? "是" : "否") << op::endl;
}

void TestEditModeCompatibility() {
    Log << "\n=== 编辑模式兼容性测试 ===" << op::endl;
    
    // 创建一个1:1比例的按钮
    Region squareRegion(0.2f, 0.2f, 0.4f, -1.0f); // 使用旧方式创建1:1比例
    Button squareButton("正方形按钮", FontID::Default, squareRegion);
    
    Log << "创建1:1比例按钮:" << op::endl;
    Region initialRegion = squareButton.GetRegion();
    Log << "初始区域: 是否1:1比例=" << (initialRegion.isAspectRatio1to1() ? "是" : "否") << op::endl;
    Log << "初始尺寸: 宽=" << initialRegion.getWidth() << ", 高=" << initialRegion.getHeight() << op::endl;
    
    // 模拟编辑模式操作
    Log << "\n模拟编辑操作:" << op::endl;
    squareButton.SetEditMode(true);
    
    // 编辑器现在应该能正确处理1:1比例
    // 测试移动操作（应该保持1:1比例）
    Region movedRegion(0.3f, 0.3f, 0.5f, 0.0f); // 移动后的位置，保持1:1比例
    movedRegion.setAspectRatio1to1(true);
    squareButton.SetRegion(movedRegion);
    
    Region afterMove = squareButton.GetRegion();
    Log << "移动后: 是否1:1比例=" << (afterMove.isAspectRatio1to1() ? "是" : "否") << op::endl;
    Log << "移动后尺寸: 宽=" << afterMove.getWidth() << ", 高=" << afterMove.getHeight() << op::endl;
    Log << "宽高是否相等: " << (abs(afterMove.getWidth() - afterMove.getHeight()) < 0.001f ? "是" : "否") << op::endl;
}

/*
 * 修复说明：
 * 
 * 1. Region类增强：
 *    - 添加了aspectRatio1to1标志位，明确标识是否使用1:1比例
 *    - 构造函数自动检测yend <= 0的情况并设置标志位
 *    - getOriginYEnd()方法现在返回正确的编辑坐标
 *    - 添加了setAspectRatio1to1()方法用于显式控制
 * 
 * 2. 编辑器兼容性：
 *    - UpdateRegionFromEdit()方法现在检测1:1比例模式
 *    - 在1:1模式下，调整操作会保持正方形
 *    - 移动操作不会破坏1:1比例特性
 *    - 最小尺寸限制考虑了1:1比例的要求
 * 
 * 3. 向后兼容性：
 *    - 原有的yend <= 0逻辑仍然有效
 *    - 现有代码无需修改即可继续工作
 *    - 新增的功能是可选的增强
 * 
 * 4. 使用建议：
 *    - 推荐使用新的显式方式：setAspectRatio1to1(true)
 *    - 旧的yend <= 0方式仍然支持，但不如新方式清晰
 *    - 编辑模式下会自动保持1:1比例特性
 */

void TestBackwardCompatibility() {
    Log << "\n=== 向后兼容性测试 ===" << op::endl;
    
    // 测试现有代码是否仍然工作
    Region legacy1(0.1f, 0.1f, 0.2f, -1.0f); // 旧方式：yend = -1
    Region legacy2(0.1f, 0.1f, 0.2f, 0.0f);  // 旧方式：yend = 0
    Region legacy3(0.1f, 0.1f, 0.2f, -0.5f); // 旧方式：yend = -0.5
    
    Log << "遗留代码测试:" << op::endl;
    Log << "yend=-1: 1:1比例=" << (legacy1.isAspectRatio1to1() ? "是" : "否") 
        << ", 高度=" << legacy1.getHeight() << op::endl;
    Log << "yend=0: 1:1比例=" << (legacy2.isAspectRatio1to1() ? "是" : "否") 
        << ", 高度=" << legacy2.getHeight() << op::endl;
    Log << "yend=-0.5: 1:1比例=" << (legacy3.isAspectRatio1to1() ? "是" : "否") 
        << ", 高度=" << legacy3.getHeight() << op::endl;
}
