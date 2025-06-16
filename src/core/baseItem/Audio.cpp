#include "core/baseItem/Audio.h"
#include "core/log.h"
#include <algorithm>

namespace core {

void Audio::channelFinishedCallback(int channel) {
    // 这是一个静态回调，我们不能直接在这里访问当前实例
    // 我们需要在类的外部处理或在使用时设置实例
}

Audio::Audio() : initialized(false) {
    Log << Level::Info << "Audio system created" << op::endl;
}

Audio::~Audio() {
    cleanup();
    Log << Level::Info << "Audio system destroyed" << op::endl;
}

bool Audio::init() {
    if (initialized) return true;
    
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        Log << Level::Error << "SDL初始化失败: " << SDL_GetError() << op::endl;
        return false;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        Log << Level::Error << "SDL_mixer初始化失败: " << Mix_GetError() << op::endl;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }
    
    // 设置通道结束回调以便跟踪活动通道
    Mix_ChannelFinished(channelFinishedCallback);
    
    initialized = true;
    Log << Level::Info << "音频系统初始化成功" << op::endl;
    return true;
}

// ===== 音乐控制函数 =====

bool Audio::loadMusic(const std::string& id, const std::string& path) {
    if (!initialized && !init()) return false;
    
    if (musics.find(id) != musics.end()) {
        Mix_FreeMusic(musics[id]);
        musics.erase(id);
    }
    
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (!music) {
        Log << Level::Error << "加载音乐失败: " << path << " - " << Mix_GetError() << op::endl;
        Log << Level::Info << "SDL_mixer提示: MP3和其他格式可能需要额外的解码器支持。请考虑转换为WAV或OGG格式。" << op::endl;
        return false;
    }
    
    musics[id] = music;
    Log << Level::Info << "音乐加载成功: " << id << " - " << path << op::endl;
    return true;
}

bool Audio::playMusic(const std::string& id, int loop) {
    if (!initialized && !init()) return false;
    
    if (musics.find(id) == musics.end()) {
        Log << Level::Error << "尝试播放未加载的音乐: " << id << op::endl;
        return false;
    }
    
    if (Mix_PlayMusic(musics[id], loop) == -1) {
        Log << Level::Error << "播放音乐失败: " << id << " - " << Mix_GetError() << op::endl;
        return false;
    }
    
    Log << Level::Info << "播放音乐: " << id << op::endl;
    return true;
}

void Audio::stopMusic() {
    if (!initialized) return;
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
        Log << Level::Info << "停止音乐播放" << op::endl;
    }
}

void Audio::pauseMusic() {
    if (!initialized) return;
    if (Mix_PlayingMusic() && !Mix_PausedMusic()) {
        Mix_PauseMusic();
        Log << Level::Info << "暂停音乐播放" << op::endl;
    }
}

void Audio::resumeMusic() {
    if (!initialized) return;
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
        Log << Level::Info << "恢复音乐播放" << op::endl;
    }
}

void Audio::setMusicVolume(int volume) {
    if (!initialized) return;
    Mix_VolumeMusic(volume); // 0-128
    Log << Level::Info << "设置音乐音量: " << volume << op::endl;
}

bool Audio::isMusicPlaying() const {
    return initialized && Mix_PlayingMusic() && !Mix_PausedMusic();
}

bool Audio::isMusicPaused() const {
    return initialized && Mix_PausedMusic();
}

// ===== 音效控制函数 =====

bool Audio::loadSound(const std::string& id, const std::string& path) {
    if (!initialized && !init()) return false;
    
    if (sounds.find(id) != sounds.end()) {
        Mix_FreeChunk(sounds[id]);
        sounds.erase(id);
    }
    
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        Log << Level::Error << "加载音效失败: " << path << " - " << Mix_GetError() << op::endl;
        return false;
    }
    
    sounds[id] = chunk;
    Log << Level::Info << "音效加载成功: " << id << " - " << path << op::endl;
    return true;
}

int Audio::playSound(const std::string& id, int loop) {
    if (!initialized && !init()) return -1;
    
    if (sounds.find(id) == sounds.end()) {
        Log << Level::Error << "尝试播放未加载的音效: " << id << op::endl;
        return -1;
    }
    
    int channel = Mix_PlayChannel(-1, sounds[id], loop);
    if (channel == -1) {
        Log << Level::Error << "播放音效失败: " << id << " - " << Mix_GetError() << op::endl;
        return -1;
    }
    
    // 跟踪此ID使用的通道
    activeChannels[id].push_back(channel);
    Log << Level::Info << "播放音效: " << id << " 在通道 " << channel << op::endl;
    
    return channel;
}

void Audio::stopSound(int channel) {
    if (!initialized || channel < 0) return;
    if (Mix_Playing(channel)) {
        Mix_HaltChannel(channel);
        Log << Level::Info << "停止音效在通道 " << channel << op::endl;
        removeChannelFromTracking(channel);
    }
}

void Audio::stopAllSounds() {
    if (!initialized) return;
    Mix_HaltChannel(-1); // -1表示所有通道
    // 清除所有通道跟踪
    for (auto& entry : activeChannels) {
        entry.second.clear();
    }
    Log << Level::Info << "停止所有音效" << op::endl;
}

void Audio::pauseSound(int channel) {
    if (!initialized || channel < 0) return;
    if (Mix_Playing(channel) && !Mix_Paused(channel)) {
        Mix_Pause(channel);
        Log << Level::Info << "暂停音效在通道 " << channel << op::endl;
    }
}

void Audio::resumeSound(int channel) {
    if (!initialized || channel < 0) return;
    if (Mix_Paused(channel)) {
        Mix_Resume(channel);
        Log << Level::Info << "恢复音效在通道 " << channel << op::endl;
    }
}

void Audio::pauseAllSounds() {
    if (!initialized) return;
    Mix_Pause(-1); // -1表示所有通道
    Log << Level::Info << "暂停所有音效" << op::endl;
}

void Audio::resumeAllSounds() {
    if (!initialized) return;
    Mix_Resume(-1); // -1表示所有通道
    Log << Level::Info << "恢复所有音效" << op::endl;
}

void Audio::setSoundVolume(const std::string& id, int volume) {
    if (!initialized || sounds.find(id) == sounds.end()) return;
    Mix_VolumeChunk(sounds[id], volume); // 0-128
    Log << Level::Info << "设置音效 " << id << " 音量: " << volume << op::endl;
}

void Audio::setChannelVolume(int channel, int volume) {
    if (!initialized || channel < 0) return;
    Mix_Volume(channel, volume); // 0-128
    Log << Level::Info << "设置通道 " << channel << " 音量: " << volume << op::endl;
}

bool Audio::isSoundPlaying(int channel) const {
    return initialized && channel >= 0 && Mix_Playing(channel) && !Mix_Paused(channel);
}

bool Audio::isSoundPaused(int channel) const {
    return initialized && channel >= 0 && Mix_Paused(channel);
}

// ===== 系统控制函数 =====

void Audio::pauseAll() {
    pauseMusic();
    pauseAllSounds();
}

void Audio::resumeAll() {
    resumeMusic();
    resumeAllSounds();
}

void Audio::stopAll() {
    stopMusic();
    stopAllSounds();
}

void Audio::cleanup() {
    if (!initialized) return;
    
    stopAll();
    
    for (auto& pair : musics) {
        Mix_FreeMusic(pair.second);
    }
    musics.clear();
    
    for (auto& pair : sounds) {
        Mix_FreeChunk(pair.second);
    }
    sounds.clear();
    
    activeChannels.clear();
    
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    initialized = false;
    
    Log << Level::Info << "音频系统已清理" << op::endl;
}

void Audio::removeChannelFromTracking(int channel) {
    for (auto& entry : activeChannels) {
        auto& channels = entry.second;
        channels.erase(std::remove(channels.begin(), channels.end(), channel), channels.end());
    }
}

void Audio::unloadMusic(const std::string& id) {
    if (!initialized || musics.find(id) == musics.end()) return;
    Mix_FreeMusic(musics[id]);
    musics.erase(id);
    Log << Level::Info << "卸载音乐: " << id << op::endl;
}

void Audio::unloadSound(const std::string& id) {
    if (!initialized || sounds.find(id) == sounds.end()) return;
    Mix_FreeChunk(sounds[id]);
    sounds.erase(id);
    Log << Level::Info << "卸载音效: " << id << op::endl;
}

} // namespace core
