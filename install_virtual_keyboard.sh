#!/bin/bash

# Linux虚拟键盘安装脚本
# 用于为本应用程序安装合适的虚拟键盘

echo "=== Linux虚拟键盘安装脚本 ==="
echo "本脚本将帮助您安装虚拟键盘以支持触屏文本输入功能"
echo

# 检测系统类型
if command -v apt-get &> /dev/null; then
    PKG_MANAGER="apt-get"
elif command -v yum &> /dev/null; then
    PKG_MANAGER="yum"
elif command -v dnf &> /dev/null; then
    PKG_MANAGER="dnf"
elif command -v pacman &> /dev/null; then
    PKG_MANAGER="pacman"
else
    echo "错误: 无法检测到支持的包管理器"
    exit 1
fi

echo "检测到包管理器: $PKG_MANAGER"
echo

# 检测已安装的虚拟键盘
echo "检测已安装的虚拟键盘:"
keyboards=("onboard" "florence" "matchbox-keyboard" "kvkbd" "cellwriter")
installed_keyboards=()

for keyboard in "${keyboards[@]}"; do
    if command -v "$keyboard" &> /dev/null; then
        echo "  ✓ $keyboard (已安装)"
        installed_keyboards+=("$keyboard")
    else
        echo "  ✗ $keyboard (未安装)"
    fi
done

echo

# 如果已有键盘，询问是否需要安装更多
if [ ${#installed_keyboards[@]} -gt 0 ]; then
    echo "您已经安装了 ${#installed_keyboards[@]} 个虚拟键盘: ${installed_keyboards[*]}"
    read -p "是否需要安装更多虚拟键盘? (y/N): " install_more
    if [[ ! "$install_more" =~ ^[Yy]$ ]]; then
        echo "跳过安装，您可以使用已安装的虚拟键盘。"
        exit 0
    fi
fi

# 推荐安装列表
echo "推荐的虚拟键盘:"
echo "1. onboard - 功能丰富，适合日常使用"
echo "2. florence - 轻量级，占用资源少"
echo "3. matchbox-keyboard - 适合嵌入式系统"
echo "4. 全部安装"
echo "5. 跳过安装"
echo

read -p "请选择要安装的选项 (1-5): " choice

case $choice in
    1)
        keyboards_to_install=("onboard")
        ;;
    2)
        keyboards_to_install=("florence")
        ;;
    3)
        keyboards_to_install=("matchbox-keyboard")
        ;;
    4)
        keyboards_to_install=("onboard" "florence" "matchbox-keyboard")
        ;;
    5)
        echo "跳过安装"
        exit 0
        ;;
    *)
        echo "无效选择，默认安装 onboard"
        keyboards_to_install=("onboard")
        ;;
esac

echo "准备安装: ${keyboards_to_install[*]}"
echo

# 根据包管理器执行安装
case $PKG_MANAGER in
    "apt-get")
        echo "更新包列表..."
        sudo apt-get update
        echo
        for keyboard in "${keyboards_to_install[@]}"; do
            echo "安装 $keyboard..."
            sudo apt-get install -y "$keyboard"
        done
        ;;
    "yum")
        for keyboard in "${keyboards_to_install[@]}"; do
            echo "安装 $keyboard..."
            sudo yum install -y "$keyboard"
        done
        ;;
    "dnf")
        for keyboard in "${keyboards_to_install[@]}"; do
            echo "安装 $keyboard..."
            sudo dnf install -y "$keyboard"
        done
        ;;
    "pacman")
        for keyboard in "${keyboards_to_install[@]}"; do
            echo "安装 $keyboard..."
            sudo pacman -S --noconfirm "$keyboard"
        done
        ;;
esac

echo
echo "安装完成！"
echo

# 再次检测安装结果
echo "验证安装结果:"
for keyboard in "${keyboards_to_install[@]}"; do
    if command -v "$keyboard" &> /dev/null; then
        echo "  ✓ $keyboard 安装成功"
    else
        echo "  ✗ $keyboard 安装失败"
    fi
done

echo
echo "使用说明:"
echo "- 现在您可以在应用程序中使用文本框功能"
echo "- 点击文本框时会自动显示虚拟键盘"
echo "- 完成输入后虚拟键盘会自动隐藏"
echo "- 如果遇到问题，请检查系统权限和依赖项"
echo

echo "安装脚本完成！"
