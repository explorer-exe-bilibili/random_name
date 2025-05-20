#include "core/screen/base.h"
#include "core/baseItem/VideoPlayer.h"
#include "core/Config.h"
#include "core/log.h"

using namespace screen;
using namespace core;

bool Screen::inited = false;
bool Screen::useVideoBackground = false;
std::shared_ptr<core::VideoPlayer> Screen::videoBackground = nullptr;

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
        } else {
            useVideoBackground = false;
            Log << Level::Error << "视频背景初始化失败，使用静态背景" << op::endl;
        }
    }
}

void Screen::Draw() {
    if (useVideoBackground && videoBackground && videoBackground->isPlaying()) {
        // 获取当前视频帧作为背景
        auto frame = videoBackground->getCurrentFrame();
        if (frame) {
            // 确保帧有纹理，如果没有则创建
            if (!frame->HasTexture()) {
                frame->CreateTextureFromBuffer();
            }
            
            // 绘制视频帧作为背景
            frame->Draw({0, 0, 1, 1});
        } else {
            // 回退到普通背景绘制
            if(background)
            background->Draw({0, 0, 1, 1});
            Log << Level::Error << "获取视频帧失败，使用静态背景" << op::endl;
        }
    } else {
        // 使用普通背景绘制
        if(background)
        background->Draw({0, 0, 1, 1});
    }
    
    for (const auto& button : buttons) {
        if(button)
            button->Draw();
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