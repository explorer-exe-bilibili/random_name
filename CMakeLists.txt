cmake_minimum_required(VERSION 3.15)

# 确保在任何其他操作之前设置 vcpkg 集成
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg target triplet")

# vcpkg 工具链文件设置（在 project() 之前）
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
    else()
        # 尝试常见路径
        if(EXISTS "D:/src/vcpkg/scripts/buildsystems/vcpkg.cmake")
            set(CMAKE_TOOLCHAIN_FILE "D:/src/vcpkg/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file")
                endif()
    endif()
endif()

# 设置项目名称和版本
project(random_name VERSION 1.0.0 LANGUAGES CXX)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加源文件
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")

# 查找所有依赖库 - 注意这里使用标准方式，无需特殊路径
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# 查找 OpenGL 库
find_package(OpenGL REQUIRED)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 包含头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE include)

# 链接所有库 - 标准方式
target_link_libraries(${PROJECT_NAME} PRIVATE 
    fmt::fmt
    glfw
    nlohmann_json::nlohmann_json
    ${OPENGL_LIBRARIES}
)

# 如果使用GLAD或其他OpenGL加载库，确保在这里添加它们
# 例如: ${GLAD_LIBRARIES}

# 如果在不同平台上需要不同的链接
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()