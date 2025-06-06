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
        set(USE_MIXED_LINKING TRUE)
        message(STATUS "Using mixed linking: Static libraries + Dynamic FFMPEG")
    endif()

    # vcpkg 工具链文件设置（在 project() 之前）
    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if(DEFINED ENV{VCPKG_ROOT})
            set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file")
        elseif(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
            set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
                CACHE STRING "Vcpkg toolchain file")
        else()
            set(CMAKE_TOOLCHAIN_FILE "")
        endif()
    endif()
endif()
# 设置项目名称和版本
project(random_name VERSION 1.0.0 LANGUAGES CXX)

# 指定 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加源文件
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")


# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

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
    find_package(glad REQUIRED)
    find_package(utf8cpp REQUIRED)
    find_package(Stb REQUIRED)
    find_package(tinyfiledialogs REQUIRED)
    # SDL2
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    
    # WebP
    pkg_check_modules(WEBP REQUIRED libwebp)
    
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
    
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${FFMPEG_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${WEBP_INCLUDE_DIRS}
        include
        ${FREETYPE_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIRS}
        ${Stb_INCLUDE_DIR}
        ${PORTABLE_FILE_DIALOGS_INCLUDE_DIRS}
    )
    target_link_directories(${PROJECT_NAME} PRIVATE
        ${FFMPEG_LIBRARY_DIRS}
    )
    
else()
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
    )

    # 链接目录
    if(FFMPEG_LIBRARY_DIRS)
        target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARY_DIRS})
    endif()

    # 链接所有库
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
endif()

# 平台特定设置
if(WIN32)
    # Windows平台特定链接
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
    
    # 如果是静态链接或混合链接，添加必要的系统库
    if(VCPKG_TARGET_TRIPLET MATCHES "static" OR USE_MIXED_LINKING)
        target_link_libraries(${PROJECT_NAME} PRIVATE
            kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 
            uuid comdlg32 advapi32 secur32 ws2_32 winmm
        )
    endif()
    
    # 添加Windows资源文件（如图标）
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/OIP-C.ico")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/OIP-C.ico" 
                       "${CMAKE_BINARY_DIR}/bin/OIP-C.ico" COPYONLY)
    endif()
    
    # 设置编译选项
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    
    # 如果是静态链接，设置运行时库
    if(VCPKG_TARGET_TRIPLET MATCHES "static")
        set_property(TARGET ${PROJECT_NAME} PROPERTY 
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
    # Linux平台特定链接
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

# 设置安装规则
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

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