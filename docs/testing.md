# 测试文档 / Testing Documentation

## 概述 / Overview

本项目包含用于验证核心功能的测试用例。测试系统使用 CMake 的 CTest 框架。

This project includes test cases to verify core functionality. The testing system uses CMake's CTest framework.

## 构建测试 / Building Tests

### 前提条件 / Prerequisites

- CMake 3.15 或更高版本 / CMake 3.15 or higher
- C++20 兼容的编译器 / C++20 compatible compiler
- 项目的所有依赖项 / All project dependencies

### 构建步骤 / Build Steps

1. 配置项目并启用测试 / Configure the project with tests enabled:

```bash
mkdir build
cd build
cmake .. -DBUILD_TESTS=ON
```

2. 构建测试可执行文件 / Build the test executables:

```bash
cmake --build . --target test_format_fix
```

或者构建所有目标 / Or build all targets:

```bash
cmake --build .
```

## 运行测试 / Running Tests

### 使用 CTest / Using CTest

在构建目录中运行所有测试 / Run all tests from the build directory:

```bash
ctest
```

详细输出 / Verbose output:

```bash
ctest -V
```

### 直接运行测试可执行文件 / Run Test Executables Directly

```bash
./test_format_fix
```

或在 Windows 上 / Or on Windows:

```bash
.\Debug\test_format_fix.exe
# 或 / or
.\Release\test_format_fix.exe
```

## 当前测试 / Current Tests

### test_format_fix

**目的 / Purpose**: 验证 `to_string_helper` 模板函数能够正确转换不同类型的数据为字符串。

**测试内容 / What it tests**:
- 整数转字符串 / Integer to string conversion
- 浮点数转字符串 / Floating-point to string conversion  
- C字符串转字符串 / C-string to string conversion

**位置 / Location**: `test_format_fix.cpp`

## 禁用测试 / Disabling Tests

如果不想构建测试，可以在配置时禁用 / To build without tests, disable them during configuration:

```bash
cmake .. -DBUILD_TESTS=OFF
```

## 添加新测试 / Adding New Tests

要添加新的测试用例 / To add a new test case:

1. 在项目根目录或 `tests/` 目录创建测试源文件
   Create a test source file in the project root or `tests/` directory

2. 在 `CMakeLists.txt` 的测试配置部分添加测试可执行文件
   Add the test executable in the test configuration section of `CMakeLists.txt`:

```cmake
add_executable(my_new_test my_new_test.cpp)
target_include_directories(my_new_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(my_new_test PRIVATE cxx_std_20)
add_test(NAME my_new_test COMMAND my_new_test)
```

3. 重新配置并构建项目
   Reconfigure and build the project

## 故障排查 / Troubleshooting

### 测试构建失败 / Test Build Fails

- 确保 C++20 支持已启用 / Ensure C++20 support is enabled
- 检查所有必需的头文件是否在包含路径中 / Check that all required headers are in the include path
- 在 Windows 上，确保使用了 UTF-8 编译选项 / On Windows, ensure UTF-8 compilation options are used

### 测试运行失败 / Test Execution Fails

- 检查输出以了解具体的失败原因 / Check the output for specific failure reasons
- 使用 `-V` 标志运行 CTest 以获取详细信息 / Run CTest with `-V` flag for detailed information
- 直接运行测试可执行文件以查看完整输出 / Run the test executable directly to see full output

## 示例输出 / Example Output

成功的测试运行 / Successful test run:

```
Int: 42
Double: 3.140000
String: test
✓ All tests passed!
```

## 注意事项 / Notes

- 测试被设计为独立的，不需要完整的应用程序初始化
  Tests are designed to be standalone and don't require full application initialization
  
- 某些测试可能需要特定的资源文件或配置
  Some tests may require specific resource files or configuration
  
- 测试应该快速运行并提供清晰的失败消息
  Tests should run quickly and provide clear failure messages
