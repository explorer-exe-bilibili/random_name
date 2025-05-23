#include "core/screen/VideoScreen.h"

#include "core/explorer.h"

namespace screen
{

VideoScreen::~VideoScreen()
{
    exit();
}

void VideoScreen::enter(int videoID)
{
    // 通过Explorer获取视频播放器
    videoPlayer = core::Explorer::getInstance()->getVideo(core::VideoID(videoID));
    if (videoPlayer)
    {
        videoPlayer->play();
    }
}

void VideoScreen::Draw()
{
    if (videoPlayer)
    {
        std::shared_ptr<core::Bitmap> currentFrame = videoPlayer->getCurrentFrame();
        if (currentFrame)
        {
            currentFrame->Draw({0, 0, 1, 1});
        }
    }
}

void VideoScreen::exit()
{
    if (videoPlayer)
    {
        videoPlayer->stop();
        videoPlayer = nullptr;
    }
}
} // namespace screen
