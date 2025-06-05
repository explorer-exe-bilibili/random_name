// 这个示例展示如何使用VideoPlayer检测视频播放完成
// 
// 新的改进逻辑：
// 1. 当读取完所有数据包时，设置reachedEOF标志，但不立即标记播放完成
// 2. 视频和音频解码线程会检查：队列为空 + 已到达EOF，才标记各自解码完成
// 3. 只有当所有解码都完成时，才真正设置isFinished = true
// 4. 这确保了所有帧都真正播放完成后才报告播放结束
//
#include "core/baseItem/Video/VideoPlayer.h"
#include "core/log.h"
#include <thread>
#include <chrono>

using namespace core;

void videoCompletionExample() {
    // 创建视频播放器实例
    VideoPlayer player;
    
    // 加载视频文件
    if (!player.load("files/video/example.mp4")) {
        Log << Level::Error << "无法加载视频文件" << op::endl;
        return;
    }
    
    // 设置为非循环播放，这样视频播放完成后会设置完成标志
    player.setLoop(false);
    
    // 开始播放
    player.play();
    Log << Level::Info << "开始播放视频..." << op::endl;
      // 监控播放状态
    while (true) {
        if (player.isCompleted()) {
            Log << Level::Info << "视频播放完成！现在所有帧都已真正播放完毕" << op::endl;
            break;
        } else if (player.isPlaying()) {
            Log << Level::Info << "视频正在播放..." << op::endl;
        } else if (player.isPaused()) {
            Log << Level::Info << "视频已暂停" << op::endl;
        }
        
        // 等待一段时间再检查
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // 停止播放器
    player.stop();
    Log << Level::Info << "示例结束" << op::endl;
}

// 演示暂停和恢复功能
void videoPauseResumeExample() {
    VideoPlayer player;
    
    if (!player.load("files/video/example.mp4")) {
        Log << Level::Error << "无法加载视频文件" << op::endl;
        return;
    }
    
    player.setLoop(false);
    player.play();
    
    // 播放5秒后暂停
    std::this_thread::sleep_for(std::chrono::seconds(5));
    player.pause();
    Log << Level::Info << "视频已暂停，状态：" << 
        (player.isPaused() ? "暂停" : "未暂停") << op::endl;
    
    // 等待3秒后恢复
    std::this_thread::sleep_for(std::chrono::seconds(3));
    player.resume();
    Log << Level::Info << "视频已恢复播放" << op::endl;
    
    // 等待播放完成
    while (!player.isCompleted()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    Log << Level::Info << "视频播放完成！" << op::endl;
    player.stop();
}
