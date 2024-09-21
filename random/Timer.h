#pragma once
#include <functional>
#include <string>
#include <thread>


class Timer
{
	std::thread *TimerThread;
	std::function<void()> CallBack;
	int WaitFor = 0;
	bool IsUsing = 0;
	bool IsPool = 0;
	bool kill = 0;
	bool Inited = 0;
public:
	std::string Describe;
	struct TimerInfo
	{
		int delay;
		bool IsUsing;
		bool IsPool;
		std::string Describe;
	};
	Timer() = default;
	~Timer();
	void setDelay(int delayMS);
	void setCallBack(std::function<void()> func);
	void setPool(bool isPool);
	
	void init();
	void start();
	void pause();
	void stop();

	TimerInfo getTimerInfo();
};