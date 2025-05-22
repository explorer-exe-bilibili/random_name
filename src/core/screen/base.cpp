#include "core/screen/base.h"
#include "core/baseItem/VideoPlayer.h"
#include "core/Config.h"
#include "core/log.h"
#include "core/render/HWAccelMonitor.h"

using namespace screen;
using namespace core;

bool Screen::inited = false;
bool Screen::useVideoBackground = false;
std::shared_ptr<core::VideoPlayer> Screen::videoBackground = nullptr;
std::string Screen::currentScreenID = "";
std::shared_ptr<Screen> Screen::currentScreen = nullptr;
std::map<std::string, std::shared_ptr<Screen>> Screen::screens;

// 注册屏幕到屏幕管理系统
void Screen::RegisterScreen(const std::string& id, std::shared_ptr<Screen> screen) {
    screens[id] = screen;
    Log << Level::Info << "已注册屏幕: " << id << op::endl;
}

// 切换到指定ID的屏幕
bool Screen::SwitchToScreen(const std::string& id) {
    if (screens.find(id) == screens.end()) {
        Log << Level::Error << "无法切换到屏幕: " << id << " (未找到)" << op::endl;
        return false;
    }
    
    // 退出当前屏幕
    if (currentScreen) {
        currentScreen->exit();
    }
    
    // 切换到新屏幕
    currentScreenID = id;
    currentScreen = screens[id];
    currentScreen->enter();
    
    Log << Level::Info << "切换到屏幕: " << id << op::endl;
    return true;
}

void Screen::initBackground() {
    if(inited)return;
    // 初始化按钮和背景
    // 初始化视频背景
    inited=true;
    background = core::Explorer::getInstance()->getBitmap("1");
    useVideoBackground = Config::getInstance()->getBool(USE_VIDEO_BACKGROUND, true);
    if (useVideoBackground) {
        std::string videoPath = Config::getInstance()->get(VIDEO_BACKGROUND_PATH, "files/video/bg.webm");
        videoBackground = std::make_shared<VideoPlayer>();
        if (videoBackground->load(videoPath)) {
            videoBackground->setLoop(true);
            videoBackground->play();
            Log << Level::Info << "视频背景初始化成功" << op::endl;
            
            // 记录硬件加速状态
            bool isHwAccel = HWAccelMonitor::getInstance().isHardwareAccelEnabled();
            Log << Level::Info << "硬件加速状态: " << (isHwAccel ? "启用" : "禁用") << op::endl;
            
            // 添加性能监控日志
#ifdef DEBUG_MODE
            // 每5秒打印一次性能监控信息
            static std::thread monitorThread([]() {
                while (true) {
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    LogHWAccelStatus();
                }
            });
            monitorThread.detach();
#endif
        } else {
            useVideoBackground = false;
            Log << Level::Error << "视频背景初始化失败，使用静态背景" << op::endl;
        }
    }
}

void Screen::Draw() {
    if (useVideoBackground && videoBackground && videoBackground->isPlaying()) {
        // 首先尝试使用硬件加速的纹理渲染路径
        auto hwTexture = videoBackground->getCurrentFrameAsTexture();
        if (hwTexture) {
            // 使用硬件加速的纹理直接渲染
            hwTexture->Draw({0, 0, 0}, {1, 1, 0});
            
            // 在调试模式下显示硬件加速信息
#ifdef DEBUG_MODE
            static int frameCount = 0;
            static std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
            frameCount++;
            
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();
            if (duration >= 1) {
                Log << Level::Debug << "硬件加速视频: " << frameCount << " FPS" << op::endl;
                frameCount = 0;
                lastTime = currentTime;
            }
#endif
        } else {
            // 回退到软件解码路径
            auto frame = videoBackground->getCurrentFrame();
            if (frame) {
                // 确保帧有纹理，如果没有则创建
                if (!frame->HasTexture()) {
                    frame->CreateTextureFromBuffer();
                }
                
                // 绘制视频帧作为背景
                frame->Draw({0, 0, 1, 1});
                
                // 在调试模式下显示软件解码信息
#ifdef DEBUG_MODE
                static int swFrameCount = 0;
                static std::chrono::high_resolution_clock::time_point swLastTime = std::chrono::high_resolution_clock::now();
                swFrameCount++;
                
                auto swCurrentTime = std::chrono::high_resolution_clock::now();
                auto swDuration = std::chrono::duration_cast<std::chrono::seconds>(swCurrentTime - swLastTime).count();
                if (swDuration >= 1) {
                    Log << Level::Debug << "软件解码视频: " << swFrameCount << " FPS" << op::endl;
                    swFrameCount = 0;
                    swLastTime = swCurrentTime;
                }
#endif
                } else {
                    // 回退到普通背景绘制
                    if(background)
                background->Draw({0, 0, 1, 1});
                Log << Level::Error << "获取视频帧失败，使用静态背景" << op::endl;
            }
        }
        
        for (const auto& button : buttons) {
            if(button)
                button->Draw();
        }
    }
}
bool Screen::Click(int x, int y) {
    bool return_value = false;
    for (const auto& button : buttons) {
        if(button)
            if(button->OnClick(Point(x, y, false)))
            {
                return_value = true;
            }
    }
    return return_value;
}