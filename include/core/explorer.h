#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/Font.h"

namespace core
{
class Explorer {
    std::shared_ptr<GLFWwindow> window;
    std::map<std::string, std::shared_ptr<core::Bitmap>> bitmaps;
    std::map<int, std::shared_ptr<core::Font>> fonts;
    static std::shared_ptr<core::Explorer> instance;
public:
    static std::shared_ptr<core::Explorer> getInstance() {
        if (!instance) {
            instance = std::shared_ptr<core::Explorer>(new core::Explorer());
        }
        return instance;
    }
    Explorer();
    ~Explorer();

    int init();

    Bitmap& getBitmap(const std::string& name);
    Font& getFont(int id);
    bool loadBitmap(const std::string& name, const std::string& path);
    int loadFont(const unsigned int ID, const std::string& path, bool needPreLoad = true);
    void loadImagesFromDirectory(const std::string& directory);
    void listLoadedBitmaps();
};
} // namespace core