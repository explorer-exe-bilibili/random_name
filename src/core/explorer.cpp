#include "core/explorer.h"
#include "core/Config.h"
#include "core/log.h"
#include <filesystem>
#include <algorithm>
#include <string>

using namespace core;

std::shared_ptr<Explorer> Explorer::instance = nullptr;

Explorer::Explorer()
{
    Log << Level::Info << "Explorer created" << op::endl;

    // 初始化音频子系统
    audio = std::make_shared<Audio>();

    if (init() != 0)
    {
        Log << Level::Error << "Explorer initialization failed" << op::endl;
    }
}

Explorer::~Explorer()
{
    Log << Level::Info << "Explorer destroying" << op::endl;
    videos.clear();
    bitmaps.clear();
    fonts.clear();
    audio.reset();
}

bool Explorer::loadBitmap(const std::string &name, const std::string &path)
{
    Log << Level::Info << "Loading bitmap: " << name << " from path: " << path << op::endl;

    return ErrorRecovery::executeWithRetry<bool>([this, &name, &path]() {
        // 检查内存压力
        MemoryMonitor::getInstance().isMemoryPressure();
        
        // 创建新的Bitmap实例
        auto bitmap = std::make_shared<Bitmap>();

        // 尝试加载图像文件
        if (bitmap->Load(path))
        {
            // 如果成功加载，将其存储在映射表中
            Name_bitmaps[name] = bitmap;
            Log << Level::Info << "Bitmap loaded successfully: " << name << " (width: " << bitmap->getWidth()
                << ", height: " << bitmap->getHeight()
                << ", texture valid: " << (*bitmap ? "true" : "false") << ")" << op::endl;
            return true;
        }
        else
        {
            Log << Level::Error << "Failed to load bitmap: " << name << " from path: " << path << op::endl;
            throw std::runtime_error("Bitmap loading failed: " + name);
        }    }, {.maxRetries = 2, .baseDelay = std::chrono::milliseconds(500)}, 
       ErrorRecovery::ErrorType::MEMORY_ALLOCATION);
}

bool Explorer::loadBitmap(BitmapID id, const std::string &path)
{
    Log << Level::Info << "Loading bitmap: " << static_cast<int>(id) << " from path: " << path << op::endl;

    try
    {
        // 创建新的Bitmap实例
        auto bitmap = std::make_shared<Bitmap>();

        // 尝试加载图像文件
        if (bitmap->Load(path))
        {
            // 如果成功加载，将其存储在映射表中
            bitmaps[id] = bitmap;
            Log << Level::Info << "Bitmap loaded successfully: " << static_cast<int>(id) << " (width: " << bitmap->getWidth()
                << ", height: " << bitmap->getHeight()
                << ", texture valid: " << (*bitmap ? "true" : "false") << ")" << op::endl;
            return true;
        }
        else
        {
            Log << Level::Error << "Failed to load bitmap: " << static_cast<int>(id) << " from path: " << path << op::endl;
        }
    }
    catch (const std::exception &e)
    {
        Log << Level::Error << "Exception while loading bitmap: " << static_cast<int>(id)
            << " from path: " << path << " - " << e.what() << op::endl;
    }

    return false;
}

void Explorer::listLoadedBitmaps()
{
    Log << Level::Info << "=== Loaded Bitmaps ===" << op::endl;
    for (const auto& [id, bitmap] : bitmaps) 
    {
        Log << Level::Info << "Bitmap ID " << static_cast<int>(id) 
            << " (" << bitmap->getWidth() << "x" << bitmap->getHeight() << ")" << op::endl;
    }
    for (const auto& [name, bitmap] : Name_bitmaps) 
    {
        Log << Level::Info << "Bitmap Name " << name 
            << " (" << bitmap->getWidth() << "x" << bitmap->getHeight() << ")" << op::endl;
    }
    Log << Level::Info << "======================" << op::endl;
}

int Explorer::loadFont(FontID id, const std::string &path, bool needPreLoad)
{
    Log << Level::Info << "Loading font from path: " << path << op::endl;
    try {
        auto font = std::make_shared<Font>(path, needPreLoad);
        if (!font->isLoaded())
        {
            throw std::runtime_error("Font failed to load properly");
        }
        fonts[id] = font;
        Log << Level::Info << "Font loaded successfully: " << static_cast<int>(id) << op::endl;
        return static_cast<int>(id);
    } catch (const std::exception &e) {
        Log << Level::Warn << "Failed to load font from path: " << path << " - " << e.what() << op::endl;
        Log << Level::Info << "Attempting to load system font as fallback..." << op::endl;
        
        // 定义跨平台的字体列表
        std::vector<std::string> fallbackFonts;
        
        #ifdef _WIN32
        // Windows 系统字体
        fallbackFonts = {
            "C:\\Windows\\Fonts\\msyh.ttc",     // 微软雅黑
            "C:\\Windows\\Fonts\\simsun.ttc",   // 宋体
            "C:\\Windows\\Fonts\\simhei.ttf",   // 黑体
            "C:\\Windows\\Fonts\\msyhl.ttc",    // 微软雅黑细体
            "C:\\Windows\\Fonts\\arial.ttf",    // Arial
            "C:\\Windows\\Fonts\\segoeui.ttf"   // Segoe UI
        };
        #elif defined(__APPLE__)
        // macOS 系统字体
        fallbackFonts = {
            "/System/Library/Fonts/PingFang.ttc",      // 苹方
            "/Library/Fonts/Arial Unicode.ttf",        // Arial Unicode
            "/System/Library/Fonts/STHeiti Light.ttc", // 华文黑体
            "/System/Library/Fonts/AppleSDGothicNeo.ttc" // Apple SD Gothic Neo
        };
        #else
        // Linux 系统字体
        fallbackFonts = {
            "/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf",
            "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
            "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
            "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf"
        };
        #endif
        
        // 使用堆栈中的备选字体
        for (const auto& systemFont : fallbackFonts) {
            try {
                Log << Level::Info << "Trying system font: " << systemFont << op::endl;
                auto font = std::make_shared<Font>(systemFont, needPreLoad);
                if (font->isLoaded())
                {
                    fonts[id] = font;
                    Log << Level::Info << "Fallback system font loaded successfully: " << systemFont << op::endl;
                    return static_cast<int>(id);
                }
                Log << Level::Warn << "Font created but not properly loaded: " << systemFont << op::endl;
            } catch (const std::exception &e) {
                Log << Level::Warn << "Exception loading system font: " << systemFont << " - " << e.what() << op::endl;
            }
        }
        
        // 尝试让系统自动查找一些通用字体名称
        const std::vector<std::string> genericFontNames = {
            "Arial",
            "DejaVu Sans",
            "Helvetica",
            "Liberation Sans",
            "Droid Sans"
        };
        
        for (const auto& fontName : genericFontNames) {
            try {
                Log << Level::Info << "Trying generic font name: " << fontName << op::endl;
                auto font = std::make_shared<Font>(fontName, needPreLoad);
                if (font->isLoaded())
                {
                    fonts[id] = font;
                    Log << Level::Info << "Generic font loaded successfully: " << fontName << op::endl;
                    return static_cast<int>(id);
                }
                Log << Level::Warn << "Generic font created but not properly loaded: " << fontName << op::endl;
            } catch (const std::exception &e) {
                Log << Level::Warn << "Exception loading generic font: " << fontName << " - " << e.what() << op::endl;
            }
        }
    }
    
    // 所有尝试都失败了，返回错误代码
    Log << Level::Error << "Failed to load any font for ID: " << static_cast<int>(id) << op::endl;
    return -1;
}

bool Explorer::loadVideo(VideoID id, const std::string &path)
{
    Log << Level::Info << "Loading video from path: " << path << op::endl;
    
    // 使用错误恢复机制加载视频
    return ErrorRecovery::executeWithRetry<bool>([this, id, &path]() {
        // 检查内存压力
        MemoryMonitor::getInstance().isMemoryPressure();

        auto video = std::make_shared<VideoPlayer>();
        if (video->load(path))
        {
            video->pause();
            videos[id] = video;
            Log << Level::Info << "Video loaded successfully: " << static_cast<int>(id) << op::endl;
            return true;
        }
        else
        {
            Log << Level::Error << "Failed to load video: " << static_cast<int>(id) << op::endl;
            throw std::runtime_error("Video loading failed for ID: " + std::to_string(static_cast<int>(id)));
        }
    }, {.maxRetries = 2, .baseDelay = std::chrono::milliseconds(1000)}, 
       ErrorRecovery::ErrorType::FFMPEG_DECODE);
}

bool Explorer::loadAudio(AudioID id, const std::string &path)
{
    Log << Level::Info << "Loading audio from path: " << path << op::endl;
    return getAudio()->loadMusic(AudioIDToString(id),path);
}

bool Explorer::playAudio(AudioID id, int loop)
{
    Log << Level::Info << "Playing audio: " << AudioIDToString(id) << " with loop count: " << loop << op::endl;
    if (getAudio()->isMusicPlaying())
    {
        Log << Level::Warn << "Audio is already playing, stopping current audio first." << op::endl;
        getAudio()->stopMusic();
    }
    return getAudio()->playMusic(AudioIDToString(id), loop);
}

void Explorer::loadImagesFromDirectory(const std::string &directory)
{
    Log << Level::Info << "Loading images from directory: " << directory << op::endl;

    try
    {
        namespace fs = std::filesystem;
        int count = 0;

        // 获取当前工作目录
        fs::path currentPath = fs::current_path();
        Log << Level::Info << "Current working directory: " << currentPath.string() << op::endl;

        // 构建目标目录的完整路径
        fs::path dirPath = directory;

        // 检查目录是否存在
        if (!fs::exists(dirPath) || !fs::is_directory(dirPath))
        {
            Log << Level::Error << "Directory does not exist: " << dirPath.string() << op::endl;

            // 尝试使用相对路径
            dirPath = currentPath / directory;
            Log << Level::Info << "Trying relative path: " << dirPath.string() << op::endl;

            if (!fs::exists(dirPath) || !fs::is_directory(dirPath))
            {
                Log << Level::Error << "Directory still does not exist: " << dirPath.string() << op::endl;
                return;
            }
        }

        Log << Level::Info << "Using directory path: " << dirPath.string() << op::endl;

        // 遍历目录中的所有文件
        for (const auto &entry : fs::directory_iterator(dirPath))
        {
            if (entry.is_regular_file())
            {
                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" || extension == ".webp")
                {
                    std::string filename = entry.path().stem().string();
                    std::string filepath = entry.path().string();

                    Log << Level::Info << "Found image file: " << filename << extension << op::endl;
                    BitmapID id = StringToBitmapID(filename);
                    if (id != BitmapID::Unknown)
                    {
                        Log << Level::Info << "Loaded image ID: " << static_cast<int>(id) << op::endl;

                        if (loadBitmap(id, filepath))
                        {
                            count++;
                            Log << Level::Info << "Successfully loaded image: " << filename << op::endl;
                        }
                        else
                        {
                            Log << Level::Error << "Failed to load image: " << filename << op::endl;
                        }
                    }
                }
            }
        }

        Log << Level::Info << "Loaded " << count << " images from directory: " << dirPath.string() << op::endl;
    }
    catch (const std::exception &e)
    {
        Log << Level::Error << "Error loading images from directory: " << directory
            << " - " << e.what() << op::endl;
    }
}

// ===== 多媒体相关函数实现 =====

bool Explorer::initAudio()
{
    if (!audio)
    {
        audio = std::make_shared<Audio>();
    }
    return audio->init();
}

Audio* Explorer::getAudio()
{
    if (!audio)
    {
        initAudio();
    }
    return audio.get();
}

VideoPlayer* Explorer::getVideo(VideoID id)
{
    if (videos.contains(id))
    {
        return videos[id].get();
    }
    return nullptr;
}



// 大小写不敏感的字符串比较辅助函数
constexpr bool iequals(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(a[i]) != std::tolower(b[i])) return false;
    }
    return true;
}

// 大小写不敏感的字符串转BitmapID函数
constexpr BitmapID core::StringToBitmapID(std::string_view str) {
    if (iequals(str, "Unknown")) return BitmapID::Unknown;
    if (iequals(str, "Background")) return BitmapID::Background;
    if (iequals(str, "Exit")) return BitmapID::Exit;
    if (iequals(str, "NameBg")) return BitmapID::NameBg;
    if (iequals(str, "Pink1Button")) return BitmapID::Pink1Button;
    if (iequals(str, "Blue1Button")) return BitmapID::Blue1Button;
    if (iequals(str, "Pink10Button")) return BitmapID::Pink10Button;
    if (iequals(str, "Blue10Button")) return BitmapID::Blue10Button;
    if (iequals(str, "SettingButton")) return BitmapID::SettingButton;
    if (iequals(str, "MainScreenButton")) return BitmapID::MainScreenButton;
    if (iequals(str, "PinkBall")) return BitmapID::PinkBall;
    if (iequals(str, "BlueBall")) return BitmapID::BlueBall;
    if (iequals(str, "Primogem")) return BitmapID::Primogem;
    if (iequals(str, "MasterlessDust")) return BitmapID::MasterlessDust;
    if (iequals(str, "MasterlessGlitter")) return BitmapID::MasterlessGlitter;
    if (iequals(str, "Element_AnemoBg")) return BitmapID::Element_AnemoBg;
    if (iequals(str, "Element_CryoBg")) return BitmapID::Element_CryoBg;
    if (iequals(str, "Element_DendroBg")) return BitmapID::Element_DendroBg;
    if (iequals(str, "Element_ElectroBg")) return BitmapID::Element_ElectroBg;
    if (iequals(str, "Element_HydroBg")) return BitmapID::Element_HydroBg;
    if (iequals(str, "Element_PyroBg")) return BitmapID::Element_PyroBg;
    if (iequals(str, "Weapon_3starBg")) return BitmapID::Weapon_3starBg;
    if (iequals(str, "Weapon_4starBg")) return BitmapID::Weapon_4starBg;
    if (iequals(str, "Weapon_5starBg")) return BitmapID::Weapon_5starBg;
    if (iequals(str, "Weapon_bowBg")) return BitmapID::Weapon_bowBg;
    if (iequals(str, "Weapon_claymoreBg")) return BitmapID::Weapon_claymoreBg;
    if (iequals(str, "Weapon_swordBg")) return BitmapID::Weapon_swordBg;
    if (iequals(str, "Weapon_polearmBg")) return BitmapID::Weapon_polearmBg;
    if (iequals(str, "Weapon_catalystBg")) return BitmapID::Weapon_catalystBg;
    if (iequals(str, "SettingBg")) return BitmapID::SettingBg;
    if (iequals(str,"floatWindow")) return BitmapID::floatWindow;
    if (iequals(str, "Overlay0")) return BitmapID::Overlay0;
    if (iequals(str, "Overlay1")) return BitmapID::Overlay1;
    if (iequals(str, "Overlay2")) return BitmapID::Overlay2;
    if (iequals(str, "Overlay3")) return BitmapID::Overlay3;
    if (iequals(str, "Overlay4")) return BitmapID::Overlay4;
    if (iequals(str, "Overlay5")) return BitmapID::Overlay5;
    if (iequals(str, "Overlay6")) return BitmapID::Overlay6;
    if (iequals(str, "Overlay7")) return BitmapID::Overlay7;
    if (iequals(str, "Overlay8")) return BitmapID::Overlay8;
    if (iequals(str, "Overlay9")) return BitmapID::Overlay9;
    if (iequals(str, "Overlay10")) return BitmapID::Overlay10;
    
    // 如果没有匹配，返回默认值
    return BitmapID::Unknown;
}

constexpr const char* core::AudioIDToString(AudioID id) {
    switch (id) {
        case AudioID::bgm: return "bgm";
        case AudioID::click: return "click";
        case AudioID::star3: return "star3";
        case AudioID::star4: return "star4";
        case AudioID::star5: return "star5";
        case AudioID::starfull: return "starfull";
        default: return "Unknown";
    }
}

// ================= 错误恢复相关方法实现 =================

void Explorer::cleanupVideoResources()
{
    Log << Level::Info << "Cleaning up video resources..." << op::endl;
    
    for (auto& [id, video] : videos) 
    {
        if (video) 
        {
            video->stop();
            // 视频资源会在智能指针析构时自动释放
        }
    }
    videos.clear();
    
    // 触发垃圾回收
    MemoryMonitor::getInstance().forceGarbageCollection();
    
    Log << Level::Info << "Video resources cleanup completed" << op::endl;
}

void Explorer::cleanupBitmapResources()
{
    Log << Level::Info << "Cleaning up bitmap resources..." << op::endl;
    
    size_t bitmapCount = bitmaps.size() + Name_bitmaps.size();
    
    bitmaps.clear();
    Name_bitmaps.clear();
    
    // 触发垃圾回收
    MemoryMonitor::getInstance().forceGarbageCollection();
    
    Log << Level::Info << "Cleaned up " << bitmapCount << " bitmap resources" << op::endl;
}

bool Explorer::validateResources()
{
    bool allValid = true;
    
    // 验证位图资源
    for (const auto& [id, bitmap] : bitmaps) 
    {
        if (!bitmap || !(*bitmap)) 
        {
            Log << Level::Warn << "Invalid bitmap resource for ID: " << static_cast<int>(id) << op::endl;
            allValid = false;
        }
    }
    
    for (const auto& [name, bitmap] : Name_bitmaps) 
    {
        if (!bitmap || !(*bitmap)) 
        {
            Log << Level::Warn << "Invalid bitmap resource for name: " << name << op::endl;
            allValid = false;
        }
    }
    
    // 验证视频资源
    for (const auto& [id, video] : videos) 
    {
        if (!video) 
        {
            Log << Level::Warn << "Invalid video resource for ID: " << static_cast<int>(id) << op::endl;
            allValid = false;
        }
    }
    
    // 验证字体资源
    for (const auto& [id, font] : fonts) 
    {
        if (!font) 
        {
            Log << Level::Warn << "Invalid font resource for ID: " << static_cast<int>(id) << op::endl;
            allValid = false;
        }
    }
    
    // 验证音频资源
    if (!audio) 
    {
        Log << Level::Warn << "Invalid audio resource" << op::endl;
        allValid = false;
    }
    
    if (allValid) 
    {
        Log << Level::Info << "All resources validated successfully" << op::endl;
    } 
    else 
    {
        Log << Level::Warn << "Resource validation found issues" << op::endl;
    }
    
    return allValid;
}