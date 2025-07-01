cmake_minimum_required(VERSION 3.15)

option(USE_SYSTEM_DEPS "Use system-installed dependencies" OFF)# 添加系统依赖选项

# 检查是否在GitHub Actions环境中
if(DEFINED ENV{GITHUB_ACTIONS})
    message(STATUS "Running in GitHub Actions environment")
    set(IN_GITHUB_ACTIONS TRUE)
else()
    message(STATUS "Running in local development environment")
    set(IN_GITHUB_ACTIONS FALSE)
endif()
if(NOT USE_SYSTEM_DEPS)
    message("Use VCPKG")
    # 确保在任何其他操作之前设置 vcpkg 集成
    if(NOT IN_GITHUB_ACTIONS)
        if(WIN32)
            set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Vcpkg target triplet")
        elseif(APPLE)
            set(VCPKG_TARGET_TRIPLET "x64-osx" CACHE STRING "Vcpkg target triplet")
        else()
            set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "Vcpkg target triplet")
        endif()
    else()
        if(DEFINED ENV{VCPKG_TARGET_TRIPLET})
            set(VCPKG_TARGET_TRIPLET $ENV{VCPKG_TARGET_TRIPLET} CACHE STRING "Vcpkg target triplet")
        else()
            if(WIN32)
                set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "Vcpkg target triplet")
            elseif(APPLE)
                set(VCPKG_TARGET_TRIPLET "x64-osx" CACHE STRING "Vcpkg target triplet")
            else()
                set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "Vcpkg target triplet")
            endif()
        endif()
    endif()

    # 检测是否使用混合链接模式
    set(USE_MIXED_LINKING FALSE)
    if(WIN32 AND VCPKG_TARGET_TRIPLET MATCHES "static-dynamic-ffmpeg")
        message("USE mixed linking")
        set(USE_MIXED_LINKING TRUE)
        message(STATUS "Using mixed linking: Static libraries + Dynamic FFMPEG")
    endif()

    # vcpkg 工具链文件设置（在 project() 之前）
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if(DEFINED ENV{VCPKG_ROOT})
            message("Using VCPKG_ROOT")
            set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file")
        elseif(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
            set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file")
        else()
            message("using /opt/vcpkg")
            set(CMAKE_TOOLCHAIN_FILE "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake")
        endif()
    endif()
endif()
# 设置项目名称和版本
project(random_name VERSION 2.0.1 LANGUAGES CXX)

# 定义版本相关变量
set(PROJECT_VERSION_MAJOR 2)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)
set(PROJECT_VERSION_BUILD 0)
set(PROJECT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# 获取构建信息
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP BUILD_TIME "%H:%M:%S")

# 获取 Git 信息
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    set(GIT_COMMIT_HASH "unknown")
    set(GIT_BRANCH "unknown")
endif()

# 平台检测和信息设置
if(WIN32)
    set(PLATFORM_NAME "Windows")
    set(PLATFORM_WINDOWS TRUE)
    
    # 获取 Windows 版本
    execute_process(
        COMMAND cmd /c "ver"
        OUTPUT_VARIABLE WINDOWS_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(WINDOWS_VERSION MATCHES "Version ([0-9]+\\.[0-9]+\\.[0-9]+)")
        set(PLATFORM_VERSION "${CMAKE_MATCH_1}")
    else()
        set(PLATFORM_VERSION "Unknown")
    endif()
    
elseif(APPLE)
    set(PLATFORM_NAME "macOS")
    set(PLATFORM_MACOS TRUE)
    
    # 获取 macOS 版本
    execute_process(
        COMMAND sw_vers -productVersion
        OUTPUT_VARIABLE PLATFORM_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT PLATFORM_VERSION)
        set(PLATFORM_VERSION "Unknown")
    endif()
    
elseif(UNIX)
    set(PLATFORM_NAME "Linux")
    set(PLATFORM_LINUX TRUE)
    
    # 获取 Linux 发行版信息
    if(EXISTS "/etc/os-release")
        execute_process(
            COMMAND grep "^PRETTY_NAME=" /etc/os-release
            COMMAND sed "s/PRETTY_NAME=\"\\(.*\\)\"/\\1/"
            OUTPUT_VARIABLE PLATFORM_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()
    if(NOT PLATFORM_VERSION)
        set(PLATFORM_VERSION "Unknown Linux")
    endif()
else()
    set(PLATFORM_NAME "Unknown")
    set(PLATFORM_VERSION "Unknown")
endif()

# 编译器检测
if(MSVC)
    set(COMPILER_NAME "MSVC")
    set(COMPILER_MSVC TRUE)
    set(COMPILER_VERSION "${MSVC_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_NAME "GCC")
    set(COMPILER_GCC TRUE)
    set(COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(COMPILER_NAME "Clang")
    set(COMPILER_CLANG TRUE)
    set(COMPILER_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
else()
    set(COMPILER_NAME "Unknown")
    set(COMPILER_VERSION "Unknown")
endif()

# 架构检测
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)")
        set(ARCHITECTURE "arm64")
        set(ARCH_ARM64 TRUE)
    else()
        set(ARCHITECTURE "x64")
        set(ARCH_X64 TRUE)
    endif()
else()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
        set(ARCHITECTURE "arm")
        set(ARCH_ARM TRUE)
    else()
        set(ARCHITECTURE "x86")
        set(ARCH_X86 TRUE)
    endif()
endif()

# 构建类型检测
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "Debug")
    set(BUILD_DEBUG TRUE)
else()
    set(BUILD_TYPE "Release")
    set(BUILD_RELEASE TRUE)
endif()

# 依赖管理方式检测
if(USE_SYSTEM_DEPS)
    set(USE_SYSTEM_DEPS_MACRO TRUE)
else()
    set(USE_VCPKG_MACRO TRUE)
    if(USE_MIXED_LINKING)
        set(USE_MIXED_LINKING_MACRO TRUE)
    endif()
endif()

# 配置版本头文件
configure_file(
    "${CMAKE_SOURCE_DIR}/include/version.h.in"
    "${CMAKE_BINARY_DIR}/generated/version.h"
    @ONLY
)

# Windows资源文件
if(WIN32)
    # 生成动态资源文件
    configure_file(
        "${CMAKE_SOURCE_DIR}/app.rc.in"
        "${CMAKE_BINARY_DIR}/generated/app.rc"
        @ONLY
    )
    configure_file(
        "${CMAKE_SOURCE_DIR}/upgrade.rc.in"
        "${CMAKE_BINARY_DIR}/generated/upgrade.rc"
        @ONLY
    )
    
    # 复制图标文件到生成目录，以便资源编译器能找到它们
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/random.ico"
        "${CMAKE_BINARY_DIR}/generated/random.ico"
        COPYONLY
    )
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/upgrade.ico"
        "${CMAKE_BINARY_DIR}/generated/upgrade.ico"
        COPYONLY
    )
    
    # 确保图标文件存在
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/random.ico")
        message(WARNING "Icon file random.ico not found in project root")
    endif()
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/upgrade.ico")
        message(WARNING "Icon file upgrade.ico not found in project root")
    endif()

    set(RESOURCE_FILES "${CMAKE_BINARY_DIR}/generated/app.rc")
    set(UPGRADE_RESOURCE_FILES "${CMAKE_BINARY_DIR}/generated/upgrade.rc")
else()
    set(RESOURCE_FILES "")
    set(UPGRADE_RESOURCE_FILES "")
endif()

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加源文件
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")

file(GLOB_RECURSE UPGRADE "upgrade/*.cpp" "upgrade/*.c")


# 创建主程序可执行文件
if(WIN32)
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCE_FILES})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
endif()

# 创建自动更新程序
if(WIN32)
    add_executable(upgrade ${UPGRADE} ${UPGRADE_RESOURCE_FILES})
else()
    add_executable(upgrade ${UPGRADE})
endif()

if(USE_SYSTEM_DEPS)
    message(STATUS "Using system dependencies")
    
    # 使用 pkg-config 查找系统库
    find_package(PkgConfig REQUIRED)
    
    # 查找系统安装的库
    pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale libswresample)
    find_package(glfw3 REQUIRED)
    find_package(nlohmann_json REQUIRED)
    find_package(Freetype REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(glm REQUIRED)
    find_package(CURL REQUIRED)
    
    # 修复 minizip 查找
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(MINIZIP QUIET minizip)
    
    if(NOT MINIZIP_FOUND)
        # 回退到传统查找方式
        find_library(MINIZIP_LIBRARY 
            NAMES minizip libminizip
            PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu
        )
        find_path(MINIZIP_INCLUDE_DIR 
            NAMES minizip/zip.h zip.h
            PATHS /usr/include /usr/local/include /usr/include/minizip
        )
        
        if(MINIZIP_LIBRARY AND MINIZIP_INCLUDE_DIR)
            add_library(minizip::minizip UNKNOWN IMPORTED)
            set_target_properties(minizip::minizip PROPERTIES
                IMPORTED_LOCATION "${MINIZIP_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${MINIZIP_INCLUDE_DIR}"
            )
            message(STATUS "Found minizip: ${MINIZIP_LIBRARY}")
        else()
            message(FATAL_ERROR "minizip not found. Please install libminizip-dev or zlib1g-dev")
        endif()
    else()
        # 使用 pkg-config 找到的版本
        add_library(minizip::minizip INTERFACE IMPORTED)
        set_target_properties(minizip::minizip PROPERTIES
            INTERFACE_LINK_LIBRARIES "${MINIZIP_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${MINIZIP_INCLUDE_DIRS}"
            INTERFACE_LINK_DIRECTORIES "${MINIZIP_LIBRARY_DIRS}"
        )
        message(STATUS "Found minizip via pkg-config: ${MINIZIP_LIBRARIES}")
    endif()


    # 特殊处理 GLAD - 适应 GLAD2 的变化
    find_package(glad QUIET)
    if(NOT glad_FOUND)
        # 回退到手动查找
        find_path(GLAD_INCLUDE_DIR 
            NAMES glad/glad.h
            PATHS /usr/local/include /usr/include
        )
        find_library(GLAD_LIBRARY
            NAMES glad libglad
            PATHS /usr/local/lib /usr/lib /usr/lib/aarch64-linux-gnu
        )
        
        if(GLAD_INCLUDE_DIR AND GLAD_LIBRARY)
            add_library(glad::glad STATIC IMPORTED)
            set_target_properties(glad::glad PROPERTIES
                IMPORTED_LOCATION "${GLAD_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${GLAD_INCLUDE_DIR}"
            )
            message(STATUS "Found GLAD: ${GLAD_LIBRARY}")
        else()
            message(FATAL_ERROR "GLAD not found. Please install GLAD development package.")
        endif()
    endif()
    

    # SDL2
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    
    # WebP
    pkg_check_modules(WEBP REQUIRED libwebp)
    # stb 通常是 header-only，可能需要手动处理
    find_path(STB_INCLUDE_DIR 
        NAMES stb_image.h
        PATHS /usr/include/stb /usr/local/include/stb /usr/include /usr/local/include ~
    )
    if(STB_INCLUDE_DIR)
        add_library(stb INTERFACE)
        target_include_directories(stb INTERFACE ${STB_INCLUDE_DIR})
        add_library(stb::stb ALIAS stb)
        message(STATUS "Found STB: ${STB_INCLUDE_DIR}")
    else()
        message(WARNING "STB not found, you may need to install libstb-dev")
    endif()
    
    # tinyfiledialogs - 可能需要手动安装
    find_library(TINYFILEDIALOGS_LIBRARY NAMES tinyfiledialogs)
    if(TINYFILEDIALOGS_LIBRARY)
        add_library(tinyfiledialogs::tinyfiledialogs UNKNOWN IMPORTED)
        set_target_properties(tinyfiledialogs::tinyfiledialogs PROPERTIES
            IMPORTED_LOCATION "${TINYFILEDIALOGS_LIBRARY}"
        )
    else()
        message(WARNING "tinyfiledialogs not found, some features may be disabled")
        # 创建一个空的目标作为占位符
        add_library(tinyfiledialogs::tinyfiledialogs INTERFACE)
    endif()
    # stb 通常是 header-only，可能需要手动处理
    find_path(STB_INCLUDE_DIR 
        NAMES stb_image.h
        PATHS /usr/include/stb /usr/local/include/stb /usr/include /usr/local/include ~
    )
    if(STB_INCLUDE_DIR)
        add_library(stb INTERFACE)
        target_include_directories(stb INTERFACE ${STB_INCLUDE_DIR})
        add_library(stb::stb ALIAS stb)
        message(STATUS "Found STB: ${STB_INCLUDE_DIR}")
    else()
        message(WARNING "STB not found, you may need to install libstb-dev")
    endif()
    
    # tinyfiledialogs - 可能需要手动安装
    find_library(TINYFILEDIALOGS_LIBRARY NAMES tinyfiledialogs)
    if(TINYFILEDIALOGS_LIBRARY)
        add_library(tinyfiledialogs::tinyfiledialogs UNKNOWN IMPORTED)
        set_target_properties(tinyfiledialogs::tinyfiledialogs PROPERTIES
            IMPORTED_LOCATION "${TINYFILEDIALOGS_LIBRARY}"
        )
    else()
        message(WARNING "tinyfiledialogs not found, some features may be disabled")
        # 创建一个空的目标作为占位符
        add_library(tinyfiledialogs::tinyfiledialogs INTERFACE)
    endif()
    # 设置系统依赖的链接
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${FFMPEG_LIBRARIES}
        glfw
        nlohmann_json::nlohmann_json
        Freetype::Freetype
        ${OPENGL_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${WEBP_LIBRARIES}
        glm::glm
        glad::glad
        utf8::cpp utf8cpp::utf8cpp
        tinyfiledialogs::tinyfiledialogs
    )
    
    # 设置系统依赖的链接 - upgrade 程序（只需要基本库）
    target_link_libraries(upgrade PRIVATE
        nlohmann_json::nlohmann_json
        CURL::libcurl
        minizip::minizip  # 修改为统一的目标名称
    )
      target_include_directories(${PROJECT_NAME} PRIVATE
        ${FFMPEG_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${WEBP_INCLUDE_DIRS}
        include
        ${CMAKE_BINARY_DIR}/generated
        ${FREETYPE_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${Stb_INCLUDE_DIR}
        ${PORTABLE_FILE_DIALOGS_INCLUDE_DIRS}
    )
    
    # upgrade 程序的包含目录
    target_include_directories(upgrade PRIVATE
        ${CURL_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}/generated
    )
    
    target_link_directories(${PROJECT_NAME} PRIVATE
        ${FFMPEG_LIBRARY_DIRS}
    )
    
else()
    message(STATUS "Using vcpkg")
    # 查找依赖库 - 根据链接模式调整
    if(USE_MIXED_LINKING)
        # 混合链接模式：FFMPEG 动态，其他静态
        message(STATUS "Configuring mixed linking for Windows")
        
        # 首先尝试查找动态 FFMPEG
        set(CMAKE_FIND_LIBRARY_SUFFIXES_BACKUP ${CMAKE_FIND_LIBRARY_SUFFIXES})
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll.a" ".lib")
        
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            pkg_check_modules(FFMPEG QUIET libavcodec libavformat libavutil libswscale)
        endif()
        
        if(NOT FFMPEG_FOUND)
            find_package(FFMPEG QUIET)
        endif()
        
        # 恢复库搜索后缀
        set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_BACKUP})
        
        # 如果动态 FFMPEG 查找失败，回退到标准方式
        if(NOT FFMPEG_FOUND)
            message(WARNING "Dynamic FFMPEG not found, falling back to standard linking")
            find_package(FFMPEG REQUIRED)
        endif()
        
        # 其他库保持静态（vcpkg 会根据 triplet 自动处理）
        find_package(WebP CONFIG REQUIRED)
        find_package(glfw3 CONFIG REQUIRED)
        find_package(nlohmann_json CONFIG REQUIRED)
        find_package(Freetype CONFIG REQUIRED)
        find_package(glm CONFIG REQUIRED)
        find_package(glad CONFIG REQUIRED)
        find_package(OpenGL REQUIRED)
        find_package(utf8cpp CONFIG REQUIRED)
        find_package(SDL2 CONFIG REQUIRED)
        find_package(SDL2_mixer CONFIG REQUIRED)
        find_package(Stb REQUIRED)
        find_package(tinyfiledialogs CONFIG REQUIRED)
        find_package(CURL REQUIRED)
        find_package(minizip CONFIG REQUIRED)
        
    else()
        # 标准模式：按 triplet 设置链接
        find_package(WebP CONFIG REQUIRED)
        find_package(FFMPEG REQUIRED)
        find_package(glfw3 CONFIG REQUIRED)
        find_package(nlohmann_json CONFIG REQUIRED)
        find_package(Freetype CONFIG REQUIRED)
        find_package(glm CONFIG REQUIRED)
        find_package(glad CONFIG REQUIRED)
        find_package(OpenGL REQUIRED)
        find_package(utf8cpp CONFIG REQUIRED)
        find_package(SDL2 CONFIG REQUIRED)
        find_package(SDL2_mixer CONFIG REQUIRED)
        find_package(Stb REQUIRED)
        find_package(tinyfiledialogs CONFIG REQUIRED)
        find_package(CURL REQUIRED)
        find_package(minizip CONFIG REQUIRED)
    endif()

    # 编译定义
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:DEBUG_MODE>
    )

    # 包含头文件目录
    target_include_directories(${PROJECT_NAME} PRIVATE 
        include
        ${FFMPEG_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${Stb_INCLUDE_DIR}
        ${PORTABLE_FILE_DIALOGS_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}/generated
    )
    target_include_directories(upgrade PRIVATE 
        ${CMAKE_BINARY_DIR}/generated
    )
    # 链接目录
    if(FFMPEG_LIBRARY_DIRS)
        target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARY_DIRS})
    endif()
    # 链接所有库 - 主程序
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        glfw
        nlohmann_json::nlohmann_json
        Freetype::Freetype
        glm::glm
        glad::glad
        ${OPENGL_LIBRARIES}
        utf8::cpp utf8cpp::utf8cpp
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
        ${FFMPEG_LIBRARIES}
        WebP::webp WebP::webpdecoder WebP::webpdemux
        tinyfiledialogs::tinyfiledialogs
    )
    
    # 链接库 - upgrade 程序（只需要基本库）
    target_link_libraries(upgrade PRIVATE 
        nlohmann_json::nlohmann_json
        CURL::libcurl
        unofficial::minizip::minizip
    )
endif()

# 自动递增构建号（可选）
if(EXISTS "${CMAKE_SOURCE_DIR}/build_number.txt")
    file(READ "${CMAKE_SOURCE_DIR}/build_number.txt" BUILD_NUMBER)
    string(STRIP "${BUILD_NUMBER}" BUILD_NUMBER)
    math(EXPR BUILD_NUMBER "${BUILD_NUMBER} + 1")
else()
    set(BUILD_NUMBER 1)
endif()

file(WRITE "${CMAKE_SOURCE_DIR}/build_number.txt" "${BUILD_NUMBER}")
set(PROJECT_VERSION_BUILD ${BUILD_NUMBER})

# 平台特定设置
if(WIN32)
    # Windows平台特定链接 - 主程序
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
    
    # Windows平台特定链接 - upgrade 程序
    target_link_libraries(upgrade PRIVATE 
        kernel32 user32 wininet urlmon shell32
    )
    # 根据构建类型设置子系统
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
        $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
    )
    # upgrade程序也应用相同的规则
    target_link_options(upgrade PRIVATE
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
        $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>
        $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>
    )
    
    # 复制random图标文件到输出目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/random.ico"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/random.ico"
        COMMENT "Copying icon file"
    )

    # 复制upgrade图标文件到输出目录
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/upgrade.ico"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/upgrade.ico"
        COMMENT "Copying icon file"
    )

    # 如果是静态链接或混合链接，添加必要的系统库
    if(VCPKG_TARGET_TRIPLET MATCHES "static" OR USE_MIXED_LINKING)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 
            uuid comdlg32 advapi32 secur32 ws2_32 winmm
        )
        
        # upgrade 程序的静态链接库
        target_link_libraries(upgrade PRIVATE
            kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 
            uuid comdlg32 advapi32 secur32 ws2_32 winmm wininet urlmon
        )
    endif()

    # 设置编译选项
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    target_compile_options(upgrade PRIVATE /utf-8)
    
    # 如果是静态链接，设置运行时库
    if(VCPKG_TARGET_TRIPLET MATCHES "static")
        set_property(TARGET ${PROJECT_NAME} PROPERTY 
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        set_property(TARGET upgrade PROPERTY 
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
    
elseif(APPLE)
    # macOS平台特定链接
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
elseif(UNIX)
    # Linux平台特定链接 - 主程序
    find_package(X11 REQUIRED)
    if(X11_FOUND)
        target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
    endif()
    
    # 对于系统安装的依赖，提供备选查找方式
    if(NOT TARGET glfw)
        find_package(glfw3 REQUIRED)
        if(TARGET glfw3)
            set_target_properties(glfw3 PROPERTIES IMPORTED_GLOBAL TRUE)
            add_library(glfw ALIAS glfw3)
        endif()
    endif()
endif()
# WSL 特定设置
if(UNIX AND NOT APPLE)
    # 检测 WSL
    execute_process(
        COMMAND grep -q Microsoft /proc/version
        RESULT_VARIABLE IS_WSL
        OUTPUT_QUIET ERROR_QUIET
    )
      if(IS_WSL EQUAL 0)
        message(STATUS "检测到 WSL 环境，应用特定优化")
        
        # 添加 WSL 特定编译标志
        target_compile_definitions(${PROJECT_NAME} PRIVATE WSL_BUILD=1)
        
        # 内存安全和错误恢复相关编译选项
        target_compile_options(${PROJECT_NAME} PRIVATE 
            -O1                          # 减少优化级别以避免某些 WSL 问题
            -fno-strict-aliasing        # 避免严格别名优化导致的问题
            -fstack-protector-strong    # 栈保护
            -D_FORTIFY_SOURCE=2         # 缓冲区溢出检测
            -fno-omit-frame-pointer     # 保留帧指针用于调试
            -fsanitize=address          # 地址检测器（调试模式）
            -g                          # 调试信息
        )
        
        # 添加链接时的内存安全标志
        target_link_options(${PROJECT_NAME} PRIVATE
            -fsanitize=address          # 地址检测器链接
            -Wl,-z,relro,-z,now        # 增强安全性
        )
        
        # 链接顺序很重要
        target_link_libraries(${PROJECT_NAME} 
            ${OPENGL_LIBRARIES}
            ${GLFW_LIBRARIES}
            ${CMAKE_DL_LIBS}
            pthread
        )
    endif()
endif()

# 设置安装规则
install(TARGETS ${PROJECT_NAME} upgrade DESTINATION bin)

# 复制必要的资源文件到输出目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/files DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# 复制 DLL 文件 - 直接修复
if(WIN32 AND VCPKG_TARGET_TRIPLET MATCHES "static")
    # 静态链接：不需要复制 DLL，创建空目标
    add_custom_target(copy_dlls ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
        COMMAND ${CMAKE_COMMAND} -E echo "Static linking - no DLLs to copy"
        COMMENT "Static linking mode"
    )
elseif(WIN32)
    # 动态链接：复制 DLL
    add_custom_target(copy_dlls ALL)
    set(VCPKG_BIN_DIR "${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin")
    
    add_custom_command(TARGET copy_dlls POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
        COMMAND ${CMAKE_COMMAND} -E echo "Checking for DLLs in ${VCPKG_BIN_DIR}"
        COMMAND if exist "${VCPKG_BIN_DIR}" (
            ${CMAKE_COMMAND} -E copy_directory "${VCPKG_BIN_DIR}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
        ) else (
            echo "No DLL directory found"
        )
        COMMENT "Copying DLLs for dynamic linking"
    )
else()
    # 非 Windows 平台
    add_custom_target(copy_dlls ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Non-Windows platform - no DLLs to copy"
        COMMENT "Non-Windows platform"
    )
endif()
