#include <windows.h>

#include "config.h"
#include "ConfigItem.h"
#include "mywindows.h"

void trySetMEMMode()
{
	if (config::get(MEM) == L"1")return;
	int i = MessageBox(NULL, L"��Ƶ����ʧ�ܣ������������ڴ治�㣬�Ƿ����ڴ��ʡģʽ����������ܻ������Ƶ���ŵ��ӳ�����", L"����", MB_ICONERROR | MB_YESNO);
	if (i == IDYES)
	{
		config::replace(MEM, L"1");
		mywindows::reboot();
	}
}
