#pragma once

#include "core/screen/base.h"
#include <string>

namespace core
{
    class VideoPlayer;
}

namespace screen
{
enum class VideoID
{
    Unknown = -1,
    Single3Star,
    Single4Star,
    Single5Star,
    Group4Star,
    Group5Star,
};
class VideoScreen : public Screen
{
    core::VideoPlayer* videoPlayer=nullptr;
    void reloadButtonsRegion() override;
public:
    VideoScreen():Screen(ScreenID::Video){init();};
    ~VideoScreen();
    void init() override;

    void enter(int videoID) override;
    void Draw() override;
    void exit() override;
private:
    void changeLanguage_()override;
    // Video playback related members
};
} // namespace screen
