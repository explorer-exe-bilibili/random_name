#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include"Log.h"
#include <vector>
class web
{
private:
	static double last_file_size;
	static std::string token_value, url, header,versionfile,raw_url;
	static std::vector<std::string> filelist;
	static std::string gettoken(std::string username, std::string password);
	static std::string getversion(); 
	int downloadzip(const char* url, const char* filename);
	static std::string getdownloadurl();
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
	static std::string findMaxVersion(const std::vector<std::string>& versions);
	static std::string removeAfterpoint(const std::string& input);
	static std::vector<std::string>departfilename(std::string& input_);
	static std::vector<std::string> listpath(std::string path); 
	static size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream);
	static int progress_callback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);
public:
	static bool isok;
	web(std::string url_, std::string username, std::string password);
	bool begindownload(std::string filename);
	static std::string token();
	static std::string version();
};