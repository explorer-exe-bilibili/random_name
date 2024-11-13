#include "task.h"



bool task::processExists(const std::wstring& processName)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &pe)) {
        do {
            if (!processName.compare(pe.szExeFile)) {
                CloseHandle(snapshot);
                return true;
            }
        } while (Process32Next(snapshot, &pe));
    }

    CloseHandle(snapshot);
	return false;
}