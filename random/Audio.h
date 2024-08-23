#pragma once
#include<string>
#include<mutex>
class Audio
{
    struct AudioData {
        std::mutex mutex;
        std::condition_variable cv;
        bool has_new_data = false;
        bool should_exit = false; // �����߳��˳��ı�־
        std::string audio_path;
    };
    void audioPlayerThread(AudioData& data);

    void sendExitRequest(AudioData& data);
    void sendAudioRequest(AudioData& data, const std::string& path);
};

