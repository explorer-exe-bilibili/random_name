#define _CRT_SECURE_NO_WARNINGS
#include "Log.h"
#include <ctime>
#include <Shlwapi.h> 
#include <vector>

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
		bSuccess = CreateDirectoryA(vIter->c_str(), nullptr) ? TRUE : FALSE;
	}
	return bSuccess;
    return false;
}

Log::Log(const std::string& fileName, const bool add) {
    if (!::PathFileExistsA("files\\log")) {
		CreatedMultipleDirectory("files\\log");
    }
	std::string filepath = "files\\log\\";
	filepath += fileName;
    if (add)
        logFile.open(filepath, std::ios::app /*std::ios::app*/);
    else logFile.open(filepath, std::ios::ate /*std::ios::app*/);
}
Log::~Log() {
    logFile.close();
}
Log& Log::operator<<(const std::string& str) {
    writeToLog(str);
    return *this;
}
Log& Log::operator<<(const long double value) {
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
    time_t now = time(nullptr);
    struct tm* localTime = localtime(&now);
    char timeStr[32];
    sprintf(timeStr, "%04d-%02d-%02d %02d:%02d:%02d", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
    logFile << "[" << timeStr << "]";
    return INSIDEINT;
}
void Log::writeToLog(const std::string& message) {
    logFile << message;
}