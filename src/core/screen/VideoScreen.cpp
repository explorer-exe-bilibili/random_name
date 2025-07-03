#include "core/screen/VideoScreen.h"

#include "core/explorer.h"
#include "core/Config.h"
#include "core/log.h"

using namespace core;
namespace screen
{

VideoScreen::~VideoScreen()
{
    VideoScreen::exit();
}

void VideoScreen::init()
{
    buttons.clear();
    buttons.resize(1);
    buttons[0] = std::make_shared<Button>();
    buttons[0]->SetText("跳过>>");
    buttons[0]->SetEnableBitmap(false);
    buttons[0]->SetRegionStr(UI_REGION_SKIP);
    buttons[0]->SetAudioID(AudioID::enter);
    buttons[0]->SetFontID(FontID::Normal);
    buttons[0]->SetColor(Color(255,255,255, 255));
    buttons[0]->SetEnableText(true);
    buttons[0]->SetEnable(true);
    buttons[0]->SetFontScale(0.3f);
    buttons[0]->SetClickFunc([]() {
        if (SwitchToScreen(ScreenID::Name, nameItems.size()))
        {
            Log << Level::Info << "跳过视频，切换到Name屏幕" << op::endl;
        }
        else
        {
            Log << Level::Error << "无法切换到Name屏幕，返回主菜单" << op::endl;
            SwitchToScreen(ScreenID::MainMenu);
        }
    });
    reloadButtonsRegion();
}

void VideoScreen::enter(int mode)
{
    if(bools[boolconfig::off_video]){
        if(int(mode/10)==1){
        nameItems.clear();
        nameItems.resize(0);
        nameItems.push_back(core::NameRandomer::getInstance(mode%10)->GetNameEntry());
        }
        else{
            nameItems.clear();
            nameItems.resize(0);
            for(int i=0;i<10;i++)nameItems.push_back(core::NameRandomer::getInstance(mode%10)->GetNameEntry());
        }
        SwitchToScreen(ScreenID::Name,nameItems.size());
        return;
    }
    videoBackground->pause();
    if (videoPlayer)
    {
        videoPlayer->pause();
        videoPlayer = nullptr;
    }
    if(int(mode/10)==1){
        nameItems.clear();
        nameItems.resize(0);
        nameItems.push_back(core::NameRandomer::getInstance(mode%10)->GetNameEntry());
        if(!bools[boolconfig::no_video_preload]){
            if(nameItems[0].star==3)videoPlayer=core::Explorer::getInstance()->getVideo(core::VideoID::Signal3star);
            else if(nameItems[0].star==4)videoPlayer=core::Explorer::getInstance()->getVideo(core::VideoID::Signal4star);
            else videoPlayer=core::Explorer::getInstance()->getVideo(core::VideoID::Signal5star);
        }
        else {
            videoPlayer = new core::VideoPlayer();
            videoPlayer->setLoop(false);
            if(nameItems[0].star==3)videoPlayer->load(core::Config::getInstance()->getPath(SIGNAL_3_STAR_VIDEO_PATH));
            else if(nameItems[0].star==4)videoPlayer->load(core::Config::getInstance()->getPath(SIGNAL_4_STAR_VIDEO_PATH));
            else videoPlayer->load(core::Config::getInstance()->getPath(SIGNAL_5_STAR_VIDEO_PATH));
            videoPlayer->setVolume(core::Config::getInstance()->getInt(VOLUME));
        }
    }
    else{
        nameItems.clear();
        nameItems.resize(0);
        for(int i=0;i<10;i++)nameItems.push_back(core::NameRandomer::getInstance(mode%10)->GetNameEntry());
        int starCount;
        for(auto& i:nameItems){
            starCount=std::max(starCount,i.star);
        }
        if(!bools[boolconfig::no_video_preload]){
            if(starCount<=4)videoPlayer=core::Explorer::getInstance()->getVideo(core::VideoID::Multi4star);
            else videoPlayer=core::Explorer::getInstance()->getVideo(core::VideoID::Multi5star);
        }
        else{
            videoPlayer = new core::VideoPlayer();
            videoPlayer->setLoop(false);
            if(starCount<=4)videoPlayer->load(core::Config::getInstance()->getPath(GROUP_4_STAR_VIDEO_PATH));
            else videoPlayer->load(core::Config::getInstance()->getPath(GROUP_5_STAR_VIDEO_PATH));
            videoPlayer->setVolume(core::Config::getInstance()->getInt(VOLUME));
        }
    }
    core::Explorer::getInstance()->getAudio()->stopMusic();
    if (videoPlayer)
    {
        videoPlayer->cleanBuffer();
        videoPlayer->play();
    }
}

void VideoScreen::Draw()
{
    if (videoPlayer)
    {
        std::shared_ptr<core::Bitmap> currentFrame = videoPlayer->getCurrentFrame();
        if (currentFrame)
        {
            currentFrame->Draw({0, 0, 1, 1});
        }
        if(videoPlayer->isCompleted()){
            Log << Level::Info << "视频播放完成" << op::endl;
            if(SwitchToScreen(ScreenID::Name,nameItems.size()))
            {
                Log << Level::Info << "切换到Name屏幕" << op::endl;
            }
            else
            {
                Log << Level::Error << "无法切换到Name屏幕" << op::endl;
                SwitchToScreen(ScreenID::MainMenu);
            }
        }
    }
    for(const auto& button : buttons)
    {
        if(button)
            button->Draw();
    }
}

void VideoScreen::exit()
{    auto now = std::chrono::steady_clock::now();
    if(bools[boolconfig::off_video])return;
    if (videoPlayer)    {
        if(!bools[boolconfig::no_video_preload]){
            videoPlayer->stop();
            videoPlayer=nullptr;
        }
        else{
            videoPlayer->stop();
            delete videoPlayer;
            videoPlayer=nullptr;
        }
    }
    auto duration = std::chrono::steady_clock::now() - now;
    Log << Level::Info << "VideoScreen Exit time:" << (int)std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << op::endl;
}

void VideoScreen::reloadButtonsRegion()
{
    for(auto& b:buttons)b->resetRegion();
}
} // namespace screen
