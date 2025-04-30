#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace audio {

// 前置声明
class Sound;
class Music;

class AudioManager {
public:
    // 单例模式
    static AudioManager& getInstance();
    
    // 初始化和清理
    bool initialize();
    void shutdown();
    
    // 加载音频
    bool loadSound(const std::string& name, const std::string& filePath);
    bool loadMusic(const std::string& name, const std::string& filePath);
    
    // 播放控制
    void playSound(const std::string& name, float volume = 1.0f);
    void playMusic(const std::string& name, bool loop = true, float volume = 1.0f);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    
    // 设置音量
    void setSoundVolume(float volume); // 0.0 - 1.0
    void setMusicVolume(float volume); // 0.0 - 1.0
    
    // 静音控制
    void muteSounds(bool mute);
    void muteMusic(bool mute);
    
    // 清理资源
    void clearSounds();
    void clearMusic();
    
    // 检查播放状态
    bool isMusicPlaying() const;
    
private:
    AudioManager();
    ~AudioManager();
    
    // 禁止复制
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    
    // 内部存储
    std::unordered_map<std::string, std::unique_ptr<Sound>> sounds;
    std::unordered_map<std::string, std::unique_ptr<Music>> music;
    
    std::string currentMusic;
    float soundVolume;
    float musicVolume;
    bool soundMuted;
    bool musicMuted;
    bool initialized;
};

} // namespace audio