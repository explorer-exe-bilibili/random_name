#include "Audio.h"


void Audio::audioPlayerThread(AudioData& data)
{
    std::unique_lock<std::mutex> lock(data.mutex);

    while (true) {
        data.cv.wait(lock, [&data] { return data.has_new_data || data.should_exit; });

        if (data.should_exit) {
            break; // ����������˳���־�����˳��߳�
        }
        // ������Ƶ
        //directshow::music(data.audio_path);
        data.has_new_data = false; // ���������ݱ�־
    }
}

void Audio::sendExitRequest(AudioData& data)
{
    std::lock_guard<std::mutex> lock(data.mutex);
    data.should_exit = true; // �����˳���־
    data.cv.notify_one(); // ֪ͨ�ȴ��߳�
}

void Audio::sendAudioRequest(AudioData& data, const std::string& path)
{
    std::lock_guard<std::mutex> lock(data.mutex);
    data.audio_path = path; // ������Ƶ·��
    data.has_new_data = true; // �����������
    data.cv.notify_one(); // ֪ͨ�ȴ��߳�
}
