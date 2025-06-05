cmake_minimum_required(VERSION 3.15)

# 确保在任何其他操作之前设置 vcpkg 集成
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
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

# 设置编译定义
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE>
)

# 包含头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE 
    include
    ${FREETYPE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${Stb_INCLUDE_DIR}
    ${FFMPEG_LIBRARY_DIRS}
    ${PORTABLE_FILE_DIALOGS_INCLUDE_DIRS}
)

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
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    # 设置可执行文件名称
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "random")
    
    # 使用 POST_BUILD 命令复制图标文件，而不是 configure_file
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/OIP-C.ico")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/OIP-C.ico"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/OIP-C.ico"
            COMMENT "Copying icon file"
        )
    endif()
    
    # 复制 DLL 文件
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../vcpkg_installed/${VCPKG_TARGET_TRIPLET}/bin/*"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND_EXPAND_LISTS
    )
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
elseif(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
endif()

# 复制资源文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/files"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/files"
)

# 安装规则
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY files/ DESTINATION bin/files)