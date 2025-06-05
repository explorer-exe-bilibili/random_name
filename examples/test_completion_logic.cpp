// 测试新的播放完成检测逻辑
// 这个测试文件用于验证视频播放完成检测是否正确工作

#include "core/baseItem/Video/VideoPlayer.h"
#include "core/log.h"
#include <thread>
#include <chrono>
#include <iostream>

using namespace core;

void testCompletionLogic() {
    Log << Level::Info << "开始测试播放完成检测逻辑..." << op::endl;
    
    VideoPlayer player;
    
    // 尝试加载一个短视频文件进行测试
    // 注意：请确保这个路径指向一个真实的短视频文件
    std::string testVideoPath = "files/video/test_short.mp4";
    
    if (!player.load(testVideoPath)) {
        Log << Level::Warn << "无法加载测试视频文件: " << testVideoPath << op::endl;
        Log << Level::Info << "请将一个短的测试视频文件放在 files/video/test_short.mp4" << op::endl;
        return;
    }
    
    // 设置为非循环播放
    player.setLoop(false);
    
    Log << Level::Info << "开始播放测试视频..." << op::endl;
    player.play();
    
    auto startTime = std::chrono::steady_clock::now();
    bool eofReported = false;
    bool completionReported = false;
    
    // 监控播放状态，记录关键时间点
    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        
        if (player.isCompleted()) {
            if (!completionReported) {
                Log << Level::Info << "播放完成报告时间: " << elapsed.count() << "ms" << op::endl;
                Log << Level::Info << "✓ 播放完成检测正常工作 - 所有帧都已播放完毕" << op::endl;
                completionReported = true;
            }
            break;
        }
        
        if (player.isPlaying()) {
            // 每500ms输出一次状态
            if (elapsed.count() % 500 == 0) {
                Log << Level::Info << "播放中... 已播放: " << elapsed.count() << "ms" << op::endl;
            }
        }
        
        // 超时保护（30秒）
        if (elapsed.count() > 30000) {
            Log << Level::Error << "测试超时，可能存在问题" << op::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    player.stop();
    Log << Level::Info << "测试完成" << op::endl;
}

void testMultiplePlayback() {
    Log << Level::Info << "测试多次播放..." << op::endl;
    
    VideoPlayer player;
    std::string testVideoPath = "files/video/test_short.mp4";
    
    if (!player.load(testVideoPath)) {
        Log << Level::Warn << "跳过多次播放测试 - 无法加载视频文件" << op::endl;
        return;
    }
    
    player.setLoop(false);
    
    for (int i = 0; i < 3; i++) {
        Log << Level::Info << "第 " << (i + 1) << " 次播放..." << op::endl;
        
        player.play();
        
        // 等待播放完成
        while (!player.isCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        Log << Level::Info << "第 " << (i + 1) << " 次播放完成" << op::endl;
        player.stop();
        
        // 短暂等待
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    Log << Level::Info << "多次播放测试完成" << op::endl;
}

int main() {
    // 初始化日志系统
    Log::init();
    
    Log << Level::Info << "=== VideoPlayer 播放完成检测测试 ===" << op::endl;
    
    // 测试基本的播放完成检测
    testCompletionLogic();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 测试多次播放
    testMultiplePlayback();
    
    Log << Level::Info << "=== 所有测试完成 ===" << op::endl;
    
    return 0;
}
