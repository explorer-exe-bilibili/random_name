#include "core/explorer.h"
#include "core/log.h"

// 这个函数展示如何使用Explorer类的音频功能
void initializeAndPlayAudio() {
    auto explorer = core::Explorer::getInstance()->getAudio();
    
    // 加载音乐和音效
    explorer->loadMusic("bgm", "files/mp3/backsound.mp3");
    explorer->loadSound("click", "files/mp3/click.mp3");
    explorer->loadSound("enter", "files/mp3/enter.mp3");
    
    // 播放背景音乐（循环）
    if (!explorer->playMusic("bgm", -1)) {
        Log << Level::Warn << "背景音乐播放失败" << op::endl;
    }
    
    // 播放音效
    int clickChannel = explorer->playSound("click");
    if (clickChannel == -1) {
        Log << Level::Warn << "点击音效播放失败" << op::endl;
    }

    // 暂停背景音乐
    explorer->pauseMusic();
    
    // 过一段时间后恢复背景音乐
    // ...等待一段时间...
    explorer->resumeMusic();
    
    // 设置音量
    explorer->setMusicVolume(64); // 50% 音量
    explorer->setSoundVolume("click", 96); // 75% 音量
    
    // 停止特定音效
    explorer->stopSound(clickChannel);
    
    // 停止所有音效
    explorer->stopAllSounds();
    
    // 停止背景音乐
    explorer->stopMusic();
    
    // 暂停所有音频
    explorer->pauseAll();
    
    // 恢复所有音频
    explorer->resumeAll();
}