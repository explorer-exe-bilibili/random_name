#include "Timer.h"

Timer::~Timer()
{
	kill = 1;
	IsUsing = 0;
	if(TimerThread)if(TimerThread->joinable())TimerThread->join();
}

void Timer::setDelay(int delayMS)
{
	this->WaitFor = delayMS;
}

void Timer::setCallBack(std::function<void()> func)
{
	this->CallBack = std::move(func);
}

void Timer::setPool(bool isPool)
{
	this->IsPool = isPool;
}

void Timer::init()
{
	 TimerThread=std::make_shared<std::thread>( std::thread([this]
		{
			 while (true)
			 {
				 while (IsUsing)
				 {
					std::this_thread::sleep_for(std::chrono::milliseconds(WaitFor));
				 	if (IsUsing&&CallBack)
					CallBack();
					if (!IsPool)
					{
					IsUsing = 0;
					}
				 }
				if(kill)return;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			 }
		}
	 ));
	TimerThread->detach();
}

void Timer::start()
{
	IsUsing = 1;
}

void Timer::pause()
{
	IsUsing = 0;
}

void Timer::stop()
{
	kill = 1;
	IsUsing = 0;
}

Timer::TimerInfo Timer::getTimerInfo()
{
	TimerInfo info;
	info.delay = WaitFor;
	info.IsUsing = IsUsing;
	info.IsPool = IsPool;
	return info;
}
