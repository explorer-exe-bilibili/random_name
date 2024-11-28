﻿#include "sth2sth.h"
#include <vector>

wchar_t sth2sth::signame[10] = { 0 };

LPCWSTR sth2sth::UTF8To16(const char* utf8String)
{
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, nullptr, 0);
	wchar_t* wideString = new wchar_t[bufferSize];

	MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, wideString, bufferSize);

	return wideString;
}
std::string sth2sth::LWStostr(const LPCWSTR lpcwszStr)
{
	std::string str;
	DWORD dwMinSize = 0;
	LPSTR lpszStr = nullptr;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, nullptr, 0, nullptr, nullptr);
	if (0 == dwMinSize)
	{
		return "";
	}
	lpszStr = new char[dwMinSize];
	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, nullptr, nullptr);
	str = lpszStr;
	delete[] lpszStr;
	return str;
}
std::wstring sth2sth::Utf82Unicode(const std::string& utf8string)
{
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, nullptr, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (widesize == 0)
	{
		throw std::exception("Error in conversion.");
	}

	std::vector<wchar_t> resultstring(widesize);

	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);

	if (convresult != widesize)
	{
		throw std::exception("La falla!");
	}

	return std::wstring(&resultstring[0]);
}
std::string sth2sth::WideByte2Acsi(const std::wstring& wstrcode)
{
	int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (asciisize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::exception("Invalid UTF-8 sequence.");
	}
	if (asciisize == 0)
	{
		throw std::exception("Error in conversion.");
	}
	std::vector<char> resultstring(asciisize);
	int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, nullptr, nullptr);

	if (convresult != asciisize)
	{
		throw std::exception("La falla!");
	}

	return std::string(&resultstring[0]);
}
std::string sth2sth::UTF_82ASCII(const std::string& strUtf8Code)
{
	std::string strRet("");
	//先把 utf8 转为 unicode
	std::wstring wstr = Utf82Unicode(strUtf8Code);
	//最后把 unicode 转为 ascii
	strRet = WideByte2Acsi(wstr);
	return strRet;
}
char* sth2sth::const_char_ptr_to_char_ptr(const char* const_char_ptr) {
	return const_cast<char*>(const_char_ptr);
}
std::string sth2sth::TCHAR2STRING(TCHAR* STR)
{
	int iLen = WideCharToMultiByte(CP_UTF8, 0, STR, -1, nullptr, 0, nullptr, nullptr);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_UTF8, 0, STR, -1, chRtn, iLen, nullptr, nullptr);
	std::string str(chRtn);
	delete[] chRtn;
	return str;
}
char* sth2sth::TCHAR2CHAR(TCHAR* STR)
{
	int iLen = WideCharToMultiByte(CP_UTF8, 0, STR, -1, nullptr, 0, nullptr, nullptr);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_UTF8, 0, STR, -1, chRtn, iLen, nullptr, nullptr);
	return chRtn;
}
char sth2sth::Lstrtosingal(const LPCWSTR in) {
	LPCWSTR it = in;
	int i = 0;
	for (LPCWSTR it = in; *it != L'\0'; it++) {
		if (*it != 65279)signame[i] = *it;
		else i--;
		i++;
	}
	return i;
}
std::wstring sth2sth::str2wstr(std::string str)
{
	std::wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
	TCHAR* buffer = new TCHAR[(size_t)len + 1];
	//多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), buffer, len);
	buffer[len] = '\0';             //添加字符串结尾
	//删除缓冲区并返回值
	result.append(buffer);
	delete[] buffer;
	return result;
}
std::wstring sth2sth::str2wstru(const std::string& str)
{
	std::wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), nullptr, 0);
	TCHAR* buffer = new TCHAR[(size_t)len + 1];
	//多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), buffer, len);
	buffer[len] = '\0';//添加字符串结尾
	//删除缓冲区并返回值
	result.append(buffer);
	delete[] buffer;
	return result;
}
std::string sth2sth::G2U(const std::string& gbk)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, nullptr, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//加1用于截断字符串 
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), gbk.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, nullptr, NULL, nullptr, nullptr);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, nullptr, nullptr);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = nullptr;
	pBuf = nullptr;

	return retStr;
}

std::string sth2sth::wstr2str(std::wstring wstr)
{
	using namespace std;
	string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), buffer, len, nullptr, nullptr);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}
std::string sth2sth::wstr2stru(std::wstring wstr)
{
	using namespace std;
	string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), buffer, len, nullptr, nullptr);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}