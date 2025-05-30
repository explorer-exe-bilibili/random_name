#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/Font.h"
#include "core/baseItem/Audio.h"
#include "core/baseItem/Video/VideoPlayer.h"

namespace core
{
enum class BitmapID{
    Unknown=0,
    Background,
    Exit,
    NameBg,
    Pink1Button,
    Blue1Button,
    Pink10Button,
    Blue10Button,
    SettingButton,
    MainScreenButton,
    PinkBall,
    BlueBall,
    Primogem,
    MasterlessDust,
    MasterlessGlitter,
    Element_AnemoBg,
    Element_CryoBg,
    Element_DendroBg,
    Element_ElectroBg,
    Element_HydroBg,
    Element_PyroBg,
    Weapon_3starBg,
    Weapon_4starBg,
    Weapon_5starBg,
    Weapon_bowBg,
    Weapon_claymoreBg,
    Weapon_swordBg,
    Weapon_polearmBg,
    Weapon_catalystBg,
    floatWindow,
    Overlay0,
    Overlay1,
    Overlay2,
    Overlay3,
    Overlay4,
    Overlay5,
    Overlay6,
    Overlay7,
    Overlay8,
    Overlay9,
    Overlay10,
};
constexpr BitmapID StringToBitmapID(std::string_view str);
enum class FontID{
    Default=0,
    Normal,
    Name,
    Icon,
    Unknown
};
enum class VideoID{
    Background=0,
    Signal3star,
    Signal4star,
    Signal5star,
    Multi4star,
    Multi5star,
    Unknown
};

class Explorer {
    std::shared_ptr<GLFWwindow> window;
    std::map<BitmapID, std::shared_ptr<core::Bitmap>> bitmaps;
    std::map<std::string, std::shared_ptr<core::Bitmap>> Name_bitmaps;
    std::map<FontID, std::shared_ptr<core::Font>> fonts;
    std::map<VideoID, std::shared_ptr<core::VideoPlayer>> videos;
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

    Bitmap* getBitmap(const std::string& name){return Name_bitmaps.contains(name) ? Name_bitmaps[name].get() : nullptr;}
    Bitmap* getBitmap(BitmapID id){return bitmaps.contains(id) ? bitmaps[id].get() : nullptr;}
    Font* getFont(FontID id){return fonts.contains(id) ? fonts[id].get() : fonts[FontID::Default].get();}
    Audio* getAudio();
    VideoPlayer* getVideo(VideoID id);

    bool isBitmapLoaded(const std::string& name) const{return Name_bitmaps.contains(name);}
    bool isBitmapLoaded(BitmapID id) const{return bitmaps.contains(id);}
    bool isFontLoaded(FontID id) const{return fonts.contains(id);}
    bool isVideoLoaded(VideoID id) const{return videos.contains(id);}

    bool loadBitmap(const std::string& name, const std::string& path);
    bool loadBitmap(BitmapID id, const std::string& path);
    int loadFont(FontID id, const std::string& path, bool needPreLoad = true);
    bool loadVideo(VideoID id, const std::string& path);

    void loadImagesFromDirectory(const std::string& directory);
    void listLoadedBitmaps();
};
} // namespace core