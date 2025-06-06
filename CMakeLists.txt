cmake_minimum_required(VERSION 3.15)

# 检查是否在GitHub Actions环境中
if(DEFINED ENV{GITHUB_ACTIONS})
    message(STATUS "Running in GitHub Actions environment")
    set(IN_GITHUB_ACTIONS TRUE)
else()
    message(STATUS "Running in local development environment")
    set(IN_GITHUB_ACTIONS FALSE)
endif()

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

# 查找所有依赖库
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

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE>
)
target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})
# 包含头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE 
    include
    ${FREETYPE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${Stb_INCLUDE_DIR}
    ${PORTABLE_FILE_DIALOGS_INCLUDE_DIRS}
)
target_link_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARY_DIRS})
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
# 平台特定设置
if(WIN32)
    # Windows平台特定链接
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
    
    # 添加Windows资源文件（如图标）
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/OIP-C.ico")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/OIP-C.ico" 
                       "${CMAKE_BINARY_DIR}/bin/OIP-C.ico" COPYONLY)
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

# 使所有目标都依赖于vcpkg manifest模式中安装的所有库
add_custom_target(copy_dlls ALL)
if(WIN32)
    add_custom_command(TARGET copy_dlls POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
    )
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()