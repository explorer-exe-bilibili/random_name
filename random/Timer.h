#pragma once
#include <functional>
#include <mutex>
#include <string>


class Timer
{
	std::shared_ptr<std::function<void()>> CallBack;
	int WaitFor = 0;
	bool IsUsing = false;
	bool IsPool = false;
	bool kill = false;
public:
	struct TimerInfo
	{
		int delay;
		bool IsUsing;
		bool IsPool;
		std::string name;
	};
	std::mutex CallBackLock;
	std::string name;
	Timer() = default;
	Timer(const Timer&);
	~Timer();
	void setDelay(int delayMS);
	void setCallBack(const std::function<void()>& func);
	void setPool(bool isPool);
	
	void init();
	void start();
	void pause();
	void stop();

	TimerInfo getTimerInfo() const;

	Timer& operator=(const Timer&);
};

