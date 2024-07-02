#include "mywindows.h"
#include <mmsystem.h>
#include <cstdarg>
#include <filesystem>
#include"sth2sth.h"

#pragma comment(lib, "winmm.lib") // 链接到 Winmm.lib

std::mutex mywindows::logMutex, mywindows::randomlock;
int mywindows::screenHeight = GetSystemMetrics(SM_CYSCREEN);
int mywindows::screenWidth = GetSystemMetrics(SM_CXSCREEN);
int mywindows::WW = screenWidth;
int mywindows::WH = screenHeight;
int mywindows::windowTop;
int mywindows::windowLeft;
int mywindows::indices[10];
HWND mywindows::main_hwnd;
HWND mywindows::load_hwnd;
HWND mywindows::float_hWnd;
HWND mywindows::Quit_hwnd;
HINSTANCE mywindows::hinstance = nullptr;
Log mywindows::logf(LOGPATH, 0), mywindows::errlogf(ERR_LOGPATH, 0);
bool mywindows::debug = false;

// 创建多级目录
bool mywindows::CreatedMultipleDirectory(const std::string& direct)
{
	std::string Directoryname = direct;
	if (Directoryname[Directoryname.length() - 1] != '\\')
	{
		Directoryname.append(1, '\\');
	}
	std::vector< std::string> vpath;
	std::string strtemp;
	BOOL  bSuccess = FALSE;
	for (unsigned int i = 0; i < Directoryname.length(); i++)
	{
		if (Directoryname[i] != '\\')
		{
			strtemp.append(1, Directoryname[i]);
		}
		else
		{
			vpath.push_back(strtemp);
			strtemp.append(1, '\\');
		}
	}
	std::vector< std::string>::iterator vIter = vpath.begin();
	for (; vIter != vpath.end(); vIter++)
	{
		bSuccess = CreateDirectoryA(vIter->c_str(), nullptr) ? TRUE : FALSE;
	}
	return bSuccess;
}
//移除文件路径的文件名
void mywindows::removeFileNameFromPath(char* path) {
	// 获取字符串长度
	size_t len = strlen(path);
	// 找到最后一个'\'的位置
	char* lastSlash = strrchr(path, '\\');
	// 如果找到了最后一个'\'，并且它后面还有字符
	if (lastSlash != nullptr && lastSlash < path + len - 1) {
		// 将最后一个'\'后面的所有字符（包括它自己）替换为字符串结束符'\0'
		*lastSlash = '\0';
	}
}
void mywindows::removeFileNameFromPath(wchar_t* path) {
	// 获取字符串长度
	size_t len = wcslen(path);
	// 找到最后一个'\'的位置
	wchar_t* lastSlash = wcsrchr(path, L'\\');
	// 如果找到了最后一个'\'，并且它后面还有字符
	if (lastSlash != nullptr && lastSlash < path + len - 1) {
		// 将最后一个'\'后面的所有字符（包括它自己）替换为字符串结束符'\0'
		*lastSlash = '\0';
	}
}
void mywindows::removeFileNameFromPath(std::wstring& path) {
	// 找到最后一个'\\'的位置
	std::size_t lastSlashPos = path.find_last_of(L'\\');

	// 如果找到了最后一个'\\'
	if (lastSlashPos != std::wstring::npos) {
		// 截断字符串,将最后一个'\\'后面的所有字符删除
		path.erase(lastSlashPos );
	}
}
//INFO输出日志
void mywindows::log(const char* format, ...) {
	if (!debug) {
		if (format == nullptr)errlog("meet a void string");
		else {
			va_list args;
			va_start(args, format);
			int length = vsnprintf(nullptr, 0, format, args) + 1;
			char* buffer = new char[length];
			if (buffer == nullptr) {
				// 如果内存分配失败，输出错误信息
				errlog("memory error(log)");
				va_end(args);
				return;
			}
			vsnprintf_s(buffer, length, _TRUNCATE, format, args);
			logf << logf.pt() << "[INFO]" << buffer << logf.nl();
			delete[] buffer;
			va_end(args);
		}
	}
}
void mywindows::log(const wchar_t* format, ...) {
	if (!debug) {
		if (format == nullptr) errlog(L"meet a void string");
		else {
			va_list args;
			va_start(args, format);
			int length = _vscwprintf(format, args) + 1; // 获取格式化字符串长度
			std::wstring buffer;
			buffer.resize(length); // 分配足够的内存
			int ret = vswprintf(&buffer[0], length, format, args); // 格式化字符串
			if (ret < 0) {
				errlog(L"Failed to format string");
				va_end(args);
				return;
			}
			logf << logf.pt() << L"[INFO]" << buffer << logf.nl();
			va_end(args);
		}
	}
}
//ERROR输出日志
void mywindows::errlog(const char* format, ...) {
	if (format == nullptr)errlog("meet a void string");
	if (format == nullptr)return;
	va_list args;
	va_start(args, format);
	int length = _vscprintf(format, args) + 1;
	char* buffer = new char[length];
	vsnprintf_s(buffer, length, _TRUNCATE, format, args);
	logf << logf.pt() << "[ERROR]" << buffer << logf.nl();
	errlogf << errlogf.pt() << "[ERROR]" << buffer << errlogf.nl();
	delete[] buffer;
	va_end(args);
}
void mywindows::errlog(const wchar_t* format, ...) {
	if (format == nullptr)errlog(L"meet a void string");
	if (format == nullptr)return;
	va_list args;
	va_start(args, format);
	int length = _vscwprintf(format, args) + 1;
	std::wstring buffer;
	buffer.resize(length); // 分配足够的内存
	int ret = vswprintf(&buffer[0], length, format, args); // 格式化字符串
	if (ret < 0) {
		errlog(L"Failed to format string");
		va_end(args);
		return;
	}
	logf << logf.pt() << "[ERROR]" << buffer << logf.nl();
	errlogf << errlogf.pt() << "[ERROR]" << buffer << errlogf.nl();
	va_end(args);
}
// 函数用于查找数组中特定值的所有位置
int* mywindows::find(int* array, const int size, const int valueToFind, int* count) {
	// 分配一个动态数组来存储找到的索引
	if (indices == nullptr) {
		// 如果内存分配失败，返回 NULL
		return nullptr;
	}
	int indexCount = 0; // 用于记录找到的索引数量

	// 遍历数组，检查每个元素是否与 valueToFind 相等
	for (int i = 0; i < size; ++i) {
		if (array[i] == valueToFind) {
			indices[indexCount++] = i; // 如果找到匹配的值，将其索引添加到数组中
		}
	}

	// 更新 count 参数，表示找到的索引数量
	*count = indexCount;

	// 如果没有找到任何匹配的值，释放分配的内存并返回 NULL
	if (indexCount == 0) {
		return nullptr;
	}

	return indices; // 返回包含所有匹配索引的数组
}
// 获取字符串宽度
int mywindows::GetStringWidth(const HDC hdc, const std::wstring& str, int height) {
	RECT rect = { 0, 0, 0, 0 };
	DrawText(hdc, str.c_str(), -1, &rect, DT_CALCRECT | DT_SINGLELINE);
	return rect.right - rect.left;
}

void mywindows::reboot()
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(nullptr, szFileName, MAX_PATH); // 获取当前执行文件的路径

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;

	// 创建一个新的进程来运行当前程序
	if (CreateProcess(szFileName,   // 程序路径
	                  nullptr,         // 命令行参数
	                  nullptr,         // 进程安全属性
	                  nullptr,         // 线程安全属性
		FALSE,        // 句柄继承选项
		0,            // 创建标志
	                  nullptr,         // 使用父进程的环境块
	                  nullptr,         // 使用父进程的起始目录
		&si,          // 指向STARTUPINFO结构的指针
		&pi))         // 指向PROCESS_INFORMATION结构的指针
	{
		// 成功创建进程，关闭当前进程
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		exit(0); // 结束当前进程
	}
}
