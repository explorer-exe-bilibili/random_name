#include "core/explorer.h"
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
    bitmaps.clear();
    fonts.clear();
    audio.reset();
}

int Explorer::init()
{
    Log << Level::Info << "Explorer initialization started" << op::endl;

    // 初始化音频系统 - 注意：SDL_Init应该先于其他SDL组件调用
    if (!initAudio())
    {
        Log << Level::Warn << "音频系统初始化失败，但将继续执行" << op::endl;
    }

    loadImagesFromDirectory("files/imgs");

    // 列出所有加载成功的图像
    Log << Level::Info << "Listing all loaded images after initialization:" << op::endl;
    listLoadedBitmaps();

    // 尝试加载默认字体
    try
    {
        loadFont(0, R"(C:\Users\j1387\source\repos\explorer-exe-bilibili\random_name\files\fonts\QS.ttf)", false);
    }
    catch (const std::exception &e)
    {
        Log << Level::Error << "Failed to load font: " << e.what() << op::endl;
    }

    Log << Level::Info << "Explorer initialization finished" << op::endl;
    return 0;
}

bool Explorer::loadBitmap(const std::string &name, const std::string &path)
{
    Log << Level::Info << "Loading bitmap: " << name << " from path: " << path << op::endl;

    try
    {
        // 创建新的Bitmap实例
        auto bitmap = std::make_shared<Bitmap>();

        // 尝试加载图像文件
        if (bitmap->Load(path))
        {
            // 如果成功加载，将其存储在映射表中
            bitmaps[name] = bitmap;
            Log << Level::Info << "Bitmap loaded successfully: " << name << " (width: " << bitmap->getWidth()
                << ", height: " << bitmap->getHeight()
                << ", texture valid: " << (*bitmap ? "true" : "false") << ")" << op::endl;
            return true;
        }
        else
        {
            Log << Level::Error << "Failed to load bitmap: " << name << " from path: " << path << op::endl;
        }
    }
    catch (const std::exception &e)
    {
        Log << Level::Error << "Exception while loading bitmap: " << name
            << " from path: " << path << " - " << e.what() << op::endl;
    }

    return false;
}

Bitmap* Explorer::getBitmap(const std::string &name)
{
    if (bitmaps.find(name) == bitmaps.end())
    {
        Log << Level::Error << "Bitmap not found: " << name << op::endl;
        Log << Level::Info << "Available bitmaps: " << op::endl;
        listLoadedBitmaps();

        return nullptr;
    }
    return bitmaps[name].get();
}

void Explorer::listLoadedBitmaps()
{
    Log << Level::Info << "Listing all loaded bitmaps (" << bitmaps.size() << " total):" << op::endl;
    int index = 0;
    for (const auto &pair : bitmaps)
    {
        Log << Level::Info << "[" << index++ << "] ID: '" << pair.first << "', "
            << "size: " << pair.second->getWidth() << "x" << pair.second->getHeight() << op::endl;
    }
}

Font* Explorer::getFont(int id)
{
    if (fonts.find(id) == fonts.end())
    {
        Log << Level::Error << "Font not found: " << id << op::endl;
        return fonts[0].get(); // 返回默认的Font对象
    }
    return fonts[id].get();
}

int Explorer::loadFont(const unsigned int ID, const std::string &path, bool needPreLoad)
{
    Log << Level::Info << "Loading font from path: " << path << op::endl;
    auto font = std::make_shared<Font>(path, needPreLoad);
    fonts[ID] = font;
    Log << Level::Info << "Font loaded successfully: " << ID << op::endl;
    return ID;
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

                if (extension == ".jpg" || extension == ".jpeg" || extension == ".png")
                {
                    std::string filename = entry.path().stem().string();
                    std::string filepath = entry.path().string();

                    Log << Level::Info << "Found image file: " << filename << extension << op::endl;

                    if (loadBitmap(filename, filepath))
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

        Log << Level::Info << "Loaded " << count << " images from directory: " << dirPath.string() << op::endl;
    }
    catch (const std::exception &e)
    {
        Log << Level::Error << "Error loading images from directory: " << directory
            << " - " << e.what() << op::endl;
    }
}

// ===== 音频相关函数实现 =====

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