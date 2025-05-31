#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
struct SwsContext;
struct SwrContext;
struct AVStream;
struct FrameData;
struct AVBufferRef;

namespace core
{
class Bitmap;
class VideoPlayer
{   
    void decodeThread();
    void decodeThreadAudio();
    void decodeThreadVideo();
    std::shared_ptr<FrameData> convertFrameToFrameData(AVFrame* frame);
    void applyVolume(uint8_t* audioBuffer, int bufferSize, int channels);
    void cleanup();
public:
    VideoPlayer();
    ~VideoPlayer();

    bool load(const std::string& path);

    void play();
    void stop();
    void pause();
    void resume();
    bool isPlaying() const{return playing && !shouldExit;}
    bool isPaused() const{return !playing && !shouldExit;}
    std::shared_ptr<core::Bitmap> getCurrentFrame();

    void setLoop(bool loop);
    void setVolume(int volume);
private:
    std::atomic<bool> loop{true};
    std::atomic<bool> playing{true};
    std::atomic<bool> shouldExit{true};
    std::thread decoderThreadAudio;
    std::thread decoderThreadVideo;
    std::thread decoderThread;
    std::mutex frameMutex;
    std::shared_ptr<FrameData> currentFrameData = nullptr;
    std::atomic<bool> frameReady{false};    // SDL音频相关
    uint32_t audioDeviceID = 0;
    std::atomic<float> volume{1.0f}; // 音量范围：0.0-1.0
    
    // FFmpeg 相关
    AVFormatContext* formatContext=nullptr;
    AVCodecContext* codecContext=nullptr;
    AVCodecContext* audioCodecContext=nullptr;
    AVStream* videoStream=nullptr;
    SwsContext* swsContext=nullptr;
    SwrContext* swrContext=nullptr;
    AVFrame* frame=nullptr;
    AVFrame* rgbFrame=nullptr;
    std::queue<AVPacket*> packetVideoQueue;
    std::queue<AVPacket*> packetAudioQueue;

    // 视频文件相关
    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
    double frameDelay = 0.0;
    int width=0;
    int height=0;
    double fps=0.0;
    std::string videoPath;
};
} // namespace core

// 添加一个帧数据结构用于线程间安全传递视频帧数据
struct FrameData {
    unsigned char* data = nullptr;
    int width = 0;
    int height = 0;
    
    FrameData() = default;
    
    FrameData(int w, int h) : width(w), height(h) {
        data = new unsigned char[w * h * 3]; // RGB格式
    }
    
    ~FrameData() {
        if (data) {
            delete[] data;
            data = nullptr;
        }
    }
    
    // 禁止拷贝，只允许移动
    FrameData(const FrameData&) = delete;
    FrameData& operator=(const FrameData&) = delete;
    
    FrameData(FrameData&& other) noexcept {
        data = other.data;
        width = other.width;
        height = other.height;
        other.data = nullptr;
        other.width = 0;
        other.height = 0;
    }
    
    FrameData& operator=(FrameData&& other) noexcept {
        if (this != &other) {
            if (data) delete[] data;
            data = other.data;
            width = other.width;
            height = other.height;
            other.data = nullptr;
            other.width = 0;
            other.height = 0;
        }
        return *this;
    }
};