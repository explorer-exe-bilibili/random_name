#include "Timer.h"

#include <thread>
#include "log.h"

Log Timer_log("./files/log/Timer.log", true);

Timer::Timer(const Timer& t)
{
	kill = t.kill;
	IsUsing = t.IsUsing;
	IsPool = t.IsPool;
	name = t.name;
	WaitFor = t.WaitFor;
	CallBack = t.CallBack;
}

Timer::~Timer()
{
	Timer_log<< Timer_log.pt() << "Timer " << name << " is deleted" << std::endl;
	kill = true;
	IsUsing = false;
}

void Timer::setDelay(int delayMS)
{
	this->WaitFor = delayMS;
}

void Timer::setCallBack(const std::function<void()>& func)
{
	CallBack.reset();
	this->CallBack = std::make_shared<std::function<void()>>(func);
}

void Timer::setPool(bool isPool)
{
	this->IsPool = isPool;
}

void Timer::init()
{
	Timer_log << Timer_log.pt() << "Timer " << name << " is created" << std::endl;
	try{std::thread([this]
		{
			 while (true)
			 {
				 while (IsUsing)
				 {
					std::this_thread::sleep_for(std::chrono::milliseconds(WaitFor));
					if (IsUsing && CallBack)
					{
						if(reinterpret_cast<long long>(CallBack.get())>0xFFFFFFFFFFF)return;
						try
						{
							std::lock_guard lock(CallBackLock);
							if (CallBack) {
								CallBack->operator()();
							}
						}
						catch (const std::exception& e)
						{
							Timer_log << "Timer:" << name << "meet a error when running callback" << e.what() << std::endl;
						}
					}
					if (!IsPool)
					{
					IsUsing = false;
					}
				 }
				if(kill)return;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			 }
		}
	 ).detach();

	}
	catch (const std::exception &e)
	{
		Timer_log <<"Timer:"<<name<<"  meet a problem:"<< e.what() << std::endl;
	}
	Timer_log <<Timer_log.pt()<< "Timer " << name << " is created compete" << std::endl;
}

void Timer::start()
{
	IsUsing = true;
}

void Timer::pause()
{
	IsUsing = false;
}

void Timer::stop()
{
	kill = true;
	IsUsing = false;
}

Timer::TimerInfo Timer::getTimerInfo() const
{
	TimerInfo info;
	info.delay = WaitFor;
	info.IsUsing = IsUsing;
	info.IsPool = IsPool;
	info.name = name;
	return info;
}
