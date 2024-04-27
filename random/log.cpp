#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "Log.h"
#include <ctime>
#include <Shlwapi.h> 
#include <vector>
#include <locale>
#include <codecvt>
#include"sth2sth.h"
#include <ctime>


std::wstring Log::wrunpath = L"";
std::string Log::runpath = "";

bool Log::CreatedMultipleDirectory(const std::string& direct)
{
	std::string Directoryname = direct;
	if (Directoryname[Directoryname.length() - 1] != '\\')
	{
		Directoryname.append(1, '\\');
	}
	std::vector< std::string> vpath;
	std::string strtemp;
	BOOL  bSuccess = FALSE;
	for (int i = 0; i < Directoryname.length(); i++)
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
		bSuccess = CreateDirectoryA(vIter->c_str(), NULL) ? TRUE : FALSE;
	}
	return bSuccess;
    return false;
}
Log::Log(const std::string& fileName) {
	std::string filepath;
	filepath = runpath;
	filepath += fileName;
	CreatedMultipleDirectory(".\\files\\log");
	logFile.open(fileName, std::ios::app);
}
Log::Log(const std::string& fileName, bool add) {
	std::string filepath;
	filepath = runpath;
	filepath += fileName;
	CreatedMultipleDirectory(".\\files\\log");
	if (add)
		logFile.open(fileName, std::ios::app /*std::ios::app*/);
	else logFile.open(fileName, std::ios::ate /*std::ios::app*/);
}
Log::~Log() {
    logFile.close();
}
Log& Log::operator<<(const std::string& str) {
    writeToLog(str);
    return *this;
}
Log& Log::operator<<(const std::wstring& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string s = converter.to_bytes(str);
    writeToLog(sth2sth::UTF_82ASCII(s));
    return *this;
}
Log& operator<<(Log& log, std::ostream& (*f)(std::ostream&)) {
	// 先将新行字符写入日志文件
	log.writeToLog("\n");

	// 然后刷新日志文件的缓冲区
	log.logFile.flush();

	// 最后返回 log 对象,以支持链式调用
	return log;
}
Log& Log::operator<<(long double value) {
	if (value != INSIDEINT) {
		long int v;
		if (value - value / 1 == 0)
		{
			v = value;
			writeToLog(std::to_string(v));
		}
		else {
			writeToLog(std::to_string(value));
		}
	}
    return *this;
}
int Log::nl() {
    logFile << std::endl;
    return INSIDEINT;
}
int Log::pt() {
    time_t now = time(0);
    struct tm* localTime = localtime(&now);
    char timeStr[32];
    sprintf(timeStr, "%04d-%02d-%02d %02d:%02d:%02d", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    logFile << "[" << timeStr << "]";
    return INSIDEINT;
}
void Log::writeToLog(const std::string& message) {
    logFile << message;
}

