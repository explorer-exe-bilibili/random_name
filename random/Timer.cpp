#include "Timer.h"

Timer::~Timer()
{

	kill = 1;
	IsUsing = 0;
	if (TimerThread) {
		while (TimerThread->joinable())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		delete TimerThread;
	}
}

void Timer::setDelay(int delayMS)
{
	this->WaitFor = delayMS;
}

void Timer::setCallBack(std::function<void()> func)
{
	this->CallBack = func;
}

void Timer::setPool(bool isPool)
{
	this->IsPool = isPool;
}

void Timer::init()
{
	if(Inited)return;
	Inited = 1;
	if (!CallBack)return;
	if (TimerThread)return;
	TimerThread=new std::thread([this]
		{
			 while (true)
			 {
				 while (IsUsing)
				 {
					 std::this_thread::sleep_for(std::chrono::milliseconds(WaitFor));
					 if (!CallBack)break;
					 if (Describe.size()>1000)break;
					 if (IsUsing)
						 CallBack();
					 if (!IsPool)
					 {
						 IsUsing = 0;
					 }
				 }
				if(kill)
				{
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			 }
			 Inited = 0;
		}
	 );
	TimerThread->detach();
}

void Timer::start()
{
	if (CallBack == nullptr)return;
	if (!Inited)init();
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
