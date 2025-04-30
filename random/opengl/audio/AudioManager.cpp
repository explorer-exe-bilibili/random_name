#include "AudioManager.h"

#ifdef _WIN32
// Windows平台 - 使用Windows音频API
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#else
// 其他平台 - 这里我们可以使用OpenAL或其他跨平台的音频库
// 为简单起见，这里只实现一个占位符
#endif

namespace audio {

// 音频基类
class AudioBase {
public:
    virtual ~AudioBase() = default;
    virtual bool load(const std::string& filePath) = 0;
    virtual void setVolume(float volume) = 0;
    virtual void release() = 0;
};

// 音效类的实现
class Sound : public AudioBase {
public:
    Sound() {}
    virtual ~Sound() { release(); }

    bool load(const std::string& filePath) override {
#ifdef _WIN32
        // Windows平台使用MCI接口
        std::wstring wideFilePath(filePath.begin(), filePath.end());
        std::wstring aliasName = L"sound_" + std::to_wstring(reinterpret_cast<uintptr_t>(this));
        
        std::wstring openCommand = L"open \"" + wideFilePath + L"\" type mpegvideo alias " + aliasName;
        if (mciSendStringW(openCommand.c_str(), nullptr, 0, nullptr) != 0) {
            return false;
        }
        
        this->filePath = filePath;
        this->alias = std::string(aliasName.begin(), aliasName.end());
        return true;
#else
        // 其他平台的实现
        return false;
#endif
    }

    bool play(float volume = 1.0f) {
#ifdef _WIN32
        // 设置音量 (MCI音量范围0-1000)
        int mciVolume = static_cast<int>(volume * 1000);
        std::wstring volumeCommand = L"setaudio " + std::wstring(alias.begin(), alias.end()) + 
                                    L" volume to " + std::to_wstring(mciVolume);
        mciSendStringW(volumeCommand.c_str(), nullptr, 0, nullptr);
        
        // 播放音效
        std::wstring playCommand = L"play " + std::wstring(alias.begin(), alias.end()) + L" from 0";
        return mciSendStringW(playCommand.c_str(), nullptr, 0, nullptr) == 0;
#else
        // 其他平台的实现
        return false;
#endif
    }

    void setVolume(float volume) override {
#ifdef _WIN32
        // MCI音量范围0-1000
        int mciVolume = static_cast<int>(volume * 1000);
        std::wstring volumeCommand = L"setaudio " + std::wstring(alias.begin(), alias.end()) + 
                                    L" volume to " + std::to_wstring(mciVolume);
        mciSendStringW(volumeCommand.c_str(), nullptr, 0, nullptr);
#else
        // 其他平台的实现
#endif
    }

    void release() override {
#ifdef _WIN32
        if (!alias.empty()) {
            std::wstring closeCommand = L"close " + std::wstring(alias.begin(), alias.end());
            mciSendStringW(closeCommand.c_str(), nullptr, 0, nullptr);
            alias.clear();
        }
#else
        // 其他平台的实现
#endif
    }

private:
    std::string filePath;
    std::string alias;
};

// 音乐类的实现（与音效类似，但支持循环播放和暂停/继续）
class Music : public AudioBase {
public:
    Music() : playing(false), paused(false) {}
    virtual ~Music() { release(); }

    bool load(const std::string& filePath) override {
#ifdef _WIN32
        // Windows平台使用MCI接口
        std::wstring wideFilePath(filePath.begin(), filePath.end());
        std::wstring aliasName = L"music_" + std::to_wstring(reinterpret_cast<uintptr_t>(this));
        
        std::wstring openCommand = L"open \"" + wideFilePath + L"\" type mpegvideo alias " + aliasName;
        if (mciSendStringW(openCommand.c_str(), nullptr, 0, nullptr) != 0) {
            return false;
        }
        
        this->filePath = filePath;
        this->alias = std::string(aliasName.begin(), aliasName.end());
        return true;
#else
        // 其他平台的实现
        return false;
#endif
    }

    bool play(bool loop = true, float volume = 1.0f) {
#ifdef _WIN32
        // 设置音量
        setVolume(volume);
        
        // 播放音乐
        std::wstring playCommand = L"play " + std::wstring(alias.begin(), alias.end()) + L" from 0";
        if (loop) {
            playCommand += L" repeat";
        }
        
        if (mciSendStringW(playCommand.c_str(), nullptr, 0, nullptr) == 0) {
            playing = true;
            paused = false;
            return true;
        }
        return false;
#else
        // 其他平台的实现
        return false;
#endif
    }

    bool stop() {
#ifdef _WIN32
        if (playing) {
            std::wstring stopCommand = L"stop " + std::wstring(alias.begin(), alias.end());
            if (mciSendStringW(stopCommand.c_str(), nullptr, 0, nullptr) == 0) {
                playing = false;
                paused = false;
                return true;
            }
        }
        return false;
#else
        // 其他平台的实现
        return false;
#endif
    }

    bool pause() {
#ifdef _WIN32
        if (playing && !paused) {
            std::wstring pauseCommand = L"pause " + std::wstring(alias.begin(), alias.end());
            if (mciSendStringW(pauseCommand.c_str(), nullptr, 0, nullptr) == 0) {
                paused = true;
                return true;
            }
        }
        return false;
#else
        // 其他平台的实现
        return false;
#endif
    }

    bool resume() {
#ifdef _WIN32
        if (playing && paused) {
            std::wstring resumeCommand = L"resume " + std::wstring(alias.begin(), alias.end());
            if (mciSendStringW(resumeCommand.c_str(), nullptr, 0, nullptr) == 0) {
                paused = false;
                return true;
            }
        }
        return false;
#else
        // 其他平台的实现
        return false;
#endif
    }

    void setVolume(float volume) override {
#ifdef _WIN32
        // MCI音量范围0-1000
        int mciVolume = static_cast<int>(volume * 1000);
        std::wstring volumeCommand = L"setaudio " + std::wstring(alias.begin(), alias.end()) + 
                                    L" volume to " + std::to_wstring(mciVolume);
        mciSendStringW(volumeCommand.c_str(), nullptr, 0, nullptr);
#else
        // 其他平台的实现
#endif
    }

    void release() override {
#ifdef _WIN32
        if (!alias.empty()) {
            if (playing) {
                std::wstring stopCommand = L"stop " + std::wstring(alias.begin(), alias.end());
                mciSendStringW(stopCommand.c_str(), nullptr, 0, nullptr);
            }
            
            std::wstring closeCommand = L"close " + std::wstring(alias.begin(), alias.end());
            mciSendStringW(closeCommand.c_str(), nullptr, 0, nullptr);
            alias.clear();
            playing = false;
            paused = false;
        }
#else
        // 其他平台的实现
#endif
    }

    bool isPlaying() const {
        return playing && !paused;
    }

private:
    std::string filePath;
    std::string alias;
    bool playing;
    bool paused;
};

// AudioManager实现
AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

AudioManager::AudioManager() 
    : soundVolume(1.0f), musicVolume(1.0f), soundMuted(false), musicMuted(false), initialized(false) {
}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::initialize() {
    if (initialized) return true;
    
#ifdef _WIN32
    // Windows平台无需特殊初始化
    initialized = true;
    return true;
#else
    // 其他平台的初始化
    initialized = false;
    return false;
#endif
}

void AudioManager::shutdown() {
    if (!initialized) return;
    
    clearSounds();
    clearMusic();
    
    initialized = false;
}

bool AudioManager::loadSound(const std::string& name, const std::string& filePath) {
    if (!initialized) return false;
    
    // 检查是否已经加载了这个音效
    if (sounds.find(name) != sounds.end()) {
        return true; // 已经加载过了
    }
    
    // 创建新音效
    auto sound = std::make_unique<Sound>();
    if (sound->load(filePath)) {
        sounds[name] = std::move(sound);
        return true;
    }
    
    return false;
}

bool AudioManager::loadMusic(const std::string& name, const std::string& filePath) {
    if (!initialized) return false;
    
    // 检查是否已经加载了这个音乐
    if (music.find(name) != music.end()) {
        return true; // 已经加载过了
    }
    
    // 创建新音乐
    auto musicObj = std::make_unique<Music>();
    if (musicObj->load(filePath)) {
        music[name] = std::move(musicObj);
        return true;
    }
    
    return false;
}

void AudioManager::playSound(const std::string& name, float volume) {
    if (!initialized || soundMuted || sounds.find(name) == sounds.end()) return;
    
    float effectiveVolume = soundMuted ? 0.0f : volume * soundVolume;
    sounds[name]->play(effectiveVolume);
}

void AudioManager::playMusic(const std::string& name, bool loop, float volume) {
    if (!initialized || music.find(name) == music.end()) return;
    
    // 先停止当前正在播放的音乐
    stopMusic();
    
    float effectiveVolume = musicMuted ? 0.0f : volume * musicVolume;
    if (music[name]->play(loop, effectiveVolume)) {
        currentMusic = name;
    }
}

void AudioManager::stopMusic() {
    if (!initialized || currentMusic.empty()) return;
    
    if (music.find(currentMusic) != music.end()) {
        music[currentMusic]->stop();
    }
    
    currentMusic.clear();
}

void AudioManager::pauseMusic() {
    if (!initialized || currentMusic.empty()) return;
    
    if (music.find(currentMusic) != music.end()) {
        music[currentMusic]->pause();
    }
}

void AudioManager::resumeMusic() {
    if (!initialized || currentMusic.empty()) return;
    
    if (music.find(currentMusic) != music.end()) {
        music[currentMusic]->resume();
    }
}

void AudioManager::setSoundVolume(float volume) {
    soundVolume = volume;
    
    // 更新所有当前已加载的音效音量
    for (auto& [name, sound] : sounds) {
        sound->setVolume(soundMuted ? 0.0f : soundVolume);
    }
}

void AudioManager::setMusicVolume(float volume) {
    musicVolume = volume;
    
    // 更新当前播放的音乐音量
    if (!currentMusic.empty() && music.find(currentMusic) != music.end()) {
        music[currentMusic]->setVolume(musicMuted ? 0.0f : musicVolume);
    }
}

void AudioManager::muteSounds(bool mute) {
    soundMuted = mute;
    
    // 更新所有当前已加载的音效音量
    for (auto& [name, sound] : sounds) {
        sound->setVolume(soundMuted ? 0.0f : soundVolume);
    }
}

void AudioManager::muteMusic(bool mute) {
    musicMuted = mute;
    
    // 更新当前播放的音乐音量
    if (!currentMusic.empty() && music.find(currentMusic) != music.end()) {
        music[currentMusic]->setVolume(musicMuted ? 0.0f : musicVolume);
    }
}

void AudioManager::clearSounds() {
    sounds.clear();
}

void AudioManager::clearMusic() {
    currentMusic.clear();
    music.clear();
}

bool AudioManager::isMusicPlaying() const {
    if (!initialized || currentMusic.empty()) return false;
    
    auto it = music.find(currentMusic);
    if (it != music.end()) {
        return static_cast<Music*>(it->second.get())->isPlaying();
    }
    
    return false;
}

} // namespace audio