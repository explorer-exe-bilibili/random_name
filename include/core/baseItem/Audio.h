#pragma once
#include <string>
#include <map>
#include <vector>
#include <SDL.h>
#include <SDL_mixer.h>

namespace core {

class Audio {
public:
    Audio();
    ~Audio();
    
    // 初始化音频系统
    bool init();
    
    // 音乐控制（背景音乐）
    bool loadMusic(const std::string& id, const std::string& path);
    bool playMusic(const std::string& id, int loop = -1);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(int volume); // 0-128
    bool isMusicPlaying() const;
    bool isMusicPaused() const;
    
    // 音效控制（短音效）
    bool loadSound(const std::string& id, const std::string& path);
    int playSound(const std::string& id, int loop = 0); // 返回通道ID
    void stopSound(int channel);
    void stopAllSounds();
    void pauseSound(int channel);
    void resumeSound(int channel);
    void pauseAllSounds();
    void resumeAllSounds();
    void setSoundVolume(const std::string& id, int volume); // 0-128
    void setChannelVolume(int channel, int volume); // 0-128
    bool isSoundPlaying(int channel) const;
    bool isSoundPaused(int channel) const;
    
    // 系统控制
    void pauseAll(); // 暂停所有音乐和音效
    void resumeAll(); // 恢复所有音乐和音效
    void stopAll(); // 停止所有音乐和音效
    void cleanup();

    void unloadMusic(const std::string& id);
    void unloadSound(const std::string& id);
    
    // 检查是否已初始化
    bool isInitialized() const { return initialized; }

private:
    std::map<std::string, Mix_Music*> musics;
    std::map<std::string, Mix_Chunk*> sounds;
    std::map<std::string, std::vector<int>> activeChannels; // 跟踪每个音效ID正在使用的通道
    bool initialized = false;
    
    // 用于通道完成回调
    static void channelFinishedCallback(int channel);
    void removeChannelFromTracking(int channel);
};

} // namespace core
