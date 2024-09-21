#include <windows.h>

#include "config.h"
#include "ConfigItem.h"
#include "mywindows.h"

void trySetMEMMode()
{
	if (config::get(MEM) == L"1")return;
	int i = MessageBox(NULL, L"视频加载失败，可能是由于内存不足，是否开启内存节省模式，开启后可能会造成视频播放的延迟增加", L"错误", MB_ICONERROR | MB_YESNO);
	if (i == IDYES)
	{
		config::replace(MEM, L"1");
		mywindows::reboot();
	}
}
