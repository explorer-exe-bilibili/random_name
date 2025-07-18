#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>
#include "core/baseItem/Bitmap.h"
#include "core/baseItem/Font.h"
#include "core/baseItem/Audio.h"
#include "core/baseItem/VideoPlayer.h"
#include "core/decrash/ErrorRecovery.h"
#include "core/decrash/MemoryMonitor.h"

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
    SettingBg,
    floatWindow,
    star3,
    star4,
    star5,
    star6,
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
enum class AudioID{
    bgm=0,
    click,
    enter,
    star3,
    star4,
    star5,
    starfull,
    Unknown
};

inline constexpr const char* AudioIDToString(AudioID id) {
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

class Explorer {
    std::shared_ptr<GLFWwindow> window;
    std::map<BitmapID, std::shared_ptr<core::Bitmap>> bitmaps;
    std::map<std::string, std::shared_ptr<core::Bitmap>> Name_bitmaps;
    // 为了支持Bitmap**，需要维护稳定的指针地址
    std::map<BitmapID, std::unique_ptr<Bitmap*>> bitmap_ptrs;
    std::map<std::string, std::unique_ptr<Bitmap*>> name_bitmap_ptrs;
    std::map<FontID, std::shared_ptr<core::Font>> fonts;
    // 为了支持Font**，需要维护稳定的指针地址
    std::map<FontID, std::unique_ptr<Font*>> font_ptrs;
    std::map<VideoID, std::shared_ptr<core::VideoPlayer>> videos;
    std::map<AudioID, bool> audioLoaded; // 用于跟踪音频是否已加载
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

    Bitmap** getBitmapPtr(const std::string& name);
    Bitmap** getBitmapPtr(BitmapID id);
    Font** getFontPtr(FontID id);
    Audio* getAudio();
    VideoPlayer* getVideo(VideoID id);

    bool isBitmapLoaded(const std::string& name) const{return Name_bitmaps.contains(name);}
    bool isBitmapLoaded(BitmapID id) const{return bitmaps.contains(id);}
    bool isFontLoaded(FontID id) const{return fonts.contains(id);}
    bool isVideoLoaded(VideoID id) const{return videos.contains(id);}
    bool isAudioLoaded(AudioID id) const{return audioLoaded.contains(id);}

    bool loadBitmap(const std::string& name, const std::string& path);
    bool loadBitmap(BitmapID id, const std::string& path);
    int loadFont(FontID id, const std::string& path, bool needPreLoad = true, unsigned int fontSize = 48);
    bool loadVideo(VideoID id, const std::string& path);
    bool loadAudio(AudioID id, const std::string& path);
    bool loadSound(AudioID id, const std::string& path);

    void unloadVideo(VideoID id);
    void unloadBitmap(const std::string& name);
    void unloadBitmap(BitmapID id);
    void unloadFont(FontID id);
    void unloadAudio(AudioID id);
    void unloadSound(AudioID id);

    void unloadAllVideo();

    bool playAudio(AudioID id, int loop = -1);
    bool playSound(AudioID id, int loop = 0);

    void loadImagesFromDirectory(const std::string& directory);
    void loadImagesFromConfig();
    void listLoadedBitmaps();

    void setVolume(char volume);
    
    // 错误恢复相关方法
    void cleanupVideoResources();
    void cleanupBitmapResources();
    bool validateResources();
};
} // namespace core