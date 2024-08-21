#include "Audio.h"


void Audio::audioPlayerThread(AudioData& data)
{
    std::unique_lock<std::mutex> lock(data.mutex);

    while (true) {
        data.cv.wait(lock, [&data] { return data.has_new_data || data.should_exit; });

        if (data.should_exit) {
            break; // 如果设置了退出标志，则退出线程
        }
        // 播放音频
        //directshow::music(data.audio_path);
        data.has_new_data = false; // 重置新数据标志
    }
}

void Audio::sendExitRequest(AudioData& data)
{
    std::lock_guard<std::mutex> lock(data.mutex);
    data.should_exit = true; // 设置退出标志
    data.cv.notify_one(); // 通知等待线程
}

void Audio::sendAudioRequest(AudioData& data, const std::string& path)
{
    std::lock_guard<std::mutex> lock(data.mutex);
    data.audio_path = path; // 设置音频路径
    data.has_new_data = true; // 标记有新数据
    data.cv.notify_one(); // 通知等待线程
}
