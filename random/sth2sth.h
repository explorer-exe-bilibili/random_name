#pragma once
#include<Windows.h>
#include<string>
class sth2sth
{
public:
	static LPCWSTR UTF8To16(const char* utf8String);
	static std::string LWStostr(LPCWSTR lpcwszStr);
	static std::wstring Utf82Unicode(const std::string& utf8string);
	static std::string WideByte2Acsi(std::wstring& wstrcode);
	static std::string UTF_82ASCII(std::string& strUtf8Code);
	static char* const_char_ptr_to_char_ptr(const char* const_char_ptr);
	static std::string TCHAR2STRING(TCHAR* STR);
	static char* TCHAR2CHAR(TCHAR* STR);
};

