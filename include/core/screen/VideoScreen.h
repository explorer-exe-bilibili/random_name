#pragma once

#include "core/screen/base.h"
#include <string>

namespace core
{
    class VideoPlayer;
}

namespace screen
{
class VideoScreen : public Screen
{
    core::VideoPlayer* videoPlayer=nullptr;
public:
    VideoScreen():Screen("Video"){};
    ~VideoScreen();

    void enter(int videoID) override;
    void Draw() override;
    void exit() override;
private:
    // Video playback related members
};
} // namespace screen
