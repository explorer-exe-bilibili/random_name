#include "core/explorer.h"
#include "core/Config.h"
#include "core/log.h"
#include "core/screen/base.h"

using namespace core;

int Explorer::init()
{
    Log << Level::Info << "Explorer initialization started" << op::endl;
    Config  *config = Config::getInstance();
    // 初始化音频系统 - 注意：SDL_Init应该先于其他SDL组件调用
    if (!initAudio())
    {
        Log << Level::Warn << "音频系统初始化失败，但将继续执行" << op::endl;
    }
    loadAudio(AudioID::bgm, config->getPath(BACKGROUND_MUSIC_PATH));
    loadSound(AudioID::click, config->getPath(CLICK_MUSIC_PATH));
    loadSound(AudioID::enter, config->getPath(ENTER_MUSIC_PATH));
    loadAudio(AudioID::star3, config->getPath(STAR_3_MUSIC_PATH));
    loadAudio(AudioID::star4, config->getPath(STAR_4_MUSIC_PATH));
    loadAudio(AudioID::star5, config->getPath(STAR_5_MUSIC_PATH));
    loadAudio(AudioID::starfull, config->getPath(STAR_FULL_MUSIC_PATH));
    Log << Level::Info << "Audio system initialized" << op::endl;

    loadImagesFromDirectory(config->getPath(IMGS_PATH));
    loadImagesFromConfig();
    // 列出所有加载成功的图像
    Log << Level::Info << "Listing all loaded images after initialization:" << op::endl;
    listLoadedBitmaps();
    // 尝试加载默认字体
    try
    {
        loadFont(FontID::Default, config->getPath(DEFAULT_FONT_PATH), false);
        loadFont(FontID::Icon, config->getPath(ICON_FONT_PATH), false);
        loadFont(FontID::Name, config->getPath(NAME_FONT_PATH), false, 150);
        loadFont(FontID::Normal, config->getPath(TEXT_FONT_PATH), false);
        Log << Level::Info << "Default fonts loaded successfully" << op::endl;
    }
    catch (const std::exception &e)
    {
        Log << Level::Error << "Failed to load font: " << e.what() << op::endl;
    }

    // 尝试加载背景视频
    if (bools[boolconfig::use_video_background])
    {
        videos[VideoID::Background] = std::make_shared<VideoPlayer>();
        if (!videos[VideoID::Background]->load(config->getPath(BACKGROUND_VIDEO_PATH)))
        {
            Log << Level::Error << "Failed to load background video" << op::endl;
            screen::Screen::setUseVideoBackground(false); // 如果加载失败，禁用视频背景
            bools[boolconfig::use_video_background] = false;
        }
        videos[VideoID::Background]->setLoop(true);
        videos[VideoID::Background]->play();
        videos[VideoID::Background]->pause();
        Log << Level::Info << "Background video loaded successfully" << op::endl;
    }
    if(!bools[boolconfig::no_video_preload]){
        // 串行加载视频，避免并发问题
        std::vector<std::pair<VideoID, std::string>> videosToLoad = {
            {VideoID::Signal3star, config->getPath(SIGNAL_3_STAR_VIDEO_PATH)},
            {VideoID::Signal4star, config->getPath(SIGNAL_4_STAR_VIDEO_PATH)},
            {VideoID::Signal5star, config->getPath(SIGNAL_5_STAR_VIDEO_PATH)},
            {VideoID::Multi4star, config->getPath(GROUP_4_STAR_VIDEO_PATH)},
            {VideoID::Multi5star, config->getPath(GROUP_5_STAR_VIDEO_PATH)}
        };
        
        for (const auto& [videoId, path] : videosToLoad) {
            try {
                videos[videoId] = std::make_shared<VideoPlayer>();
                if (videos[videoId]->load(path)) {
                    videos[videoId]->setVolume(config->getInt(VOLUME));
                    videos[videoId]->setLoop(false);
                    Log << Level::Info << "成功加载视频: " << static_cast<int>(videoId) << op::endl;
                } else {
                    Log << Level::Error << "加载视频失败: " << static_cast<int>(videoId) << op::endl;
                    videos[videoId].reset();
                }
                
                // 添加短暂延迟，避免快速连续分配
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } catch (const std::exception& e) {
                Log << Level::Error << "视频加载异常: " << e.what() << op::endl;
                videos[videoId].reset();
            }
        }
        
        Log << Level::Info << "Preloaded videos successfully" << op::endl;
    }
    Log << Level::Info << "Explorer initialization finished" << op::endl;
    return 0;
}

void Explorer::loadImagesFromConfig()
{
    Config* config = Config::getInstance();
    loadBitmap(BitmapID::Overlay0,config->getPath(OVERLAY1));
    loadBitmap(BitmapID::Overlay1,config->getPath(OVERLAY2));
    loadBitmap(BitmapID::Overlay2,config->getPath(OVERLAY3));
    loadBitmap(BitmapID::Overlay3,config->getPath(OVERLAY4));
    loadBitmap(BitmapID::floatWindow,config->getPath(FLOAT_WINDOW_IMG));
    loadBitmap(BitmapID::Background,config->getPath(BACKGROUND_IMG_PATH));
}