#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/Font.h"
#include "core/baseItem/Audio.h"

namespace core
{
class Explorer {
    std::shared_ptr<GLFWwindow> window;
    std::map<std::string, std::shared_ptr<core::Bitmap>> bitmaps;
    std::map<int, std::shared_ptr<core::Font>> fonts;
    std::shared_ptr<Audio> audio;
    static std::shared_ptr<core::Explorer> instance;

    bool initAudio();
public:
    static std::shared_ptr<Explorer> getInstance2()
    {
	    if (!instance)instance = std::shared_ptr<Explorer>(new Explorer);
        return instance;
    }
    static Explorer* getInstance() {
        if (!instance) instance = std::shared_ptr<core::Explorer>(new core::Explorer());
        return instance.get();
    }
    Explorer();
    ~Explorer();

    int init();

    Bitmap* getBitmap(const std::string& name);
    Font* getFont(int id);
    Audio* getAudio();
    bool isBitmapLoaded(const std::string& name) const{return bitmaps.contains(name);}
    bool isFontLoaded(int id) const{return fonts.contains(id);}
    bool loadBitmap(const std::string& name, const std::string& path);
    int loadFont(const unsigned int ID, const std::string& path, bool needPreLoad = true);
    void loadImagesFromDirectory(const std::string& directory);
    void listLoadedBitmaps();
};
} // namespace core