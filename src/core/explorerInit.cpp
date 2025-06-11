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
    loadAudio(AudioID::bgm, "files/music/backsound.mp3");
    loadSound(AudioID::click, "files/music/click.mp3");
    loadSound(AudioID::enter, "files/music/enter.mp3");
    loadAudio(AudioID::star3, "files/music/star3.mp3");
    loadAudio(AudioID::star4, "files/music/star4.mp3");
    loadAudio(AudioID::star5, "files/music/star5.mp3");
    loadAudio(AudioID::starfull, "files/music/starfull.mp3");
    Log << Level::Info << "Audio system initialized" << op::endl;

    loadImagesFromDirectory(config->getPath(IMGS_PATH));

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
    if (config->getBool(USE_VIDEO_BACKGROUND))
    {
        videos[VideoID::Background] = std::make_shared<VideoPlayer>();
        if (!videos[VideoID::Background]->load(config->getPath(VIDEO_BACKGROUND_PATH)))
        {
            Log << Level::Error << "Failed to load background video" << op::endl;
            screen::Screen::setUseVideoBackground(false); // 如果加载失败，禁用视频背景
        }
        videos[VideoID::Background]->setLoop(true);
        videos[VideoID::Background]->play();
        videos[VideoID::Background]->pause();
        Log << Level::Info << "Background video loaded successfully" << op::endl;
    }
    if(!config->getBool(NO_VIDEO_PRELOAD,0)){
        // 串行加载视频，避免并发问题
        std::vector<std::pair<VideoID, std::string>> videosToLoad = {
            {VideoID::Signal3star, config->getPath(SIGNAL_3_STAR_VIDEO_PATH, "files/video/signal3star.mp4")},
            {VideoID::Signal4star, config->getPath(SIGNAL_4_STAR_VIDEO_PATH, "files/video/signal4star.mp4")},
            {VideoID::Signal5star, config->getPath(SIGNAL_5_STAR_VIDEO_PATH, "files/video/signal5star.mp4")},
            {VideoID::Multi4star, config->getPath(GROUP_4_STAR_VIDEO_PATH, "files/video/multi4star.mp4")},
            {VideoID::Multi5star, config->getPath(GROUP_5_STAR_VIDEO_PATH, "files/video/multi5star.mp4")}
        };
        
        for (const auto& [videoId, path] : videosToLoad) {
            try {
                videos[videoId] = std::make_shared<VideoPlayer>();
                if (videos[videoId]->load(path)) {
                    videos[videoId]->setVolume(config->getInt(VOLUME, 50));
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
