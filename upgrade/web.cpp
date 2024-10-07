#include "web.h"
#include"json.h"
#include<curl/curl.h>
#include <sstream>
#include <regex>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace Json;

Log weblog("upgrade.log",1);

std::string web::token_value = "";
std::string web::url = "";
std::string web::header = "";
std::string web::raw_url = "";
std::string web::versionfile = "";
bool web::isok = 1;
double web::last_file_size = 0;

string web::getdownloadurl() {
	CURL* curl;
	CURLcode res;
	Value post, getvalue;
	Reader jsonreader;
	StyledWriter jsonwriter;
	string data, tmp, geturl, getdata;
#ifdef _M_IX86
	tmp = "/x86/";
#else
	tmp = "/";
#endif // _M_IX86

	tmp += versionfile;
	post["path"] = tmp.c_str();
	data = jsonwriter.write(post);
	geturl = url; geturl += "api/fs/get";
	curl = curl_easy_init(); 
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, geturl.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, header.c_str());
		headers = curl_slist_append(headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &web::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &getdata);
		res = curl_easy_perform(curl);
		if (!jsonreader.parse(getdata, getvalue)) {
			weblog << "JSON解析失败" << weblog.nl();
			isok = 0;
			curl_easy_cleanup(curl);
			return "err";
		}
		else if (getvalue["code"].asInt() == 200) {
			// 访问data对象的content数组
#ifdef _DEBUG
			cout << jsonwriter.write(getvalue) << endl;
#else
			weblog << jsonwriter.write(getvalue) << weblog.nl();

#endif // _DEBUG

			if (getvalue.isMember("data") && getvalue["data"].isObject() && getvalue["data"]["raw_url"].isString()) {
				raw_url = getvalue["data"]["raw_url"].asString();
#ifdef _DEBUG
				cout << raw_url << endl;
#else
				weblog << raw_url << weblog.nl();
#endif // _DEBUG

				curl_easy_cleanup(curl);
				return raw_url;
			}
			else {
#ifdef _DEBUG
				cout << "遇到未知错误" << endl;
#else
				weblog << "遇到未知错误" << weblog.nl();
#endif // _DEBUG
				isok = 0;
				curl_easy_cleanup(curl);
				return "err";
			}
		}
	}
	curl_easy_cleanup(curl);
	return "err";
}
string web::getversion() {
#ifdef _M_IX86
	std::vector<std::string> listfile = listpath("/x86/");
#else
	std::vector<std::string> listfile = listpath("/");
#endif // _M_IX86

	for (const auto& match : listfile) {
		weblog << match <<weblog.nl();
	}
	string highestfile=findMaxVersion(listfile);
	versionfile = highestfile;
	return "";
}
std::string web::version()
{
	return versionfile;
}
std::string web::removeAfterpoint(const std::string& input) {
	// 找到第一个 "."
	size_t dashPos = input.find_last_of(".");
	if (dashPos != std::string::npos) {
		// 如果找到了 ".", 裁切掉 "." 后面的内容
		string m = input.substr(0, dashPos);
		dashPos = m.find("-");
		if (dashPos != std::string::npos) {
			m= m.substr(0, dashPos);
			return m;
		}
		else return m;
	}
	else {
		// 如果未找到 ".", 直接返回原字符串
		return input;
	}
}
std::vector<std::string> web::departfilename(std::string& input_) {
	string input = input_;
	// 正则表达式，匹配格式为 "数字.数字.数字.zip" 的字符串
	std::regex pattern(R"(\d+\.\d+\.\d+(-\w+)?\.zip)");

	// 使用正则表达式查找所有匹配的片段
	std::sregex_iterator begin(input.begin(), input.end(), pattern);
	std::sregex_iterator end;
	std::vector<std::string> matches;

	// 遍历所有匹配项，并将它们添加到matches向量中
	for (std::sregex_iterator i = begin; i != end; ++i) {
		string m = i->str();
		matches.push_back(m);
	}
	return matches;
}
std::vector<std::string> web::listpath(string path) {
	CURL* curl;
	CURLcode res;
	string data, getdata, list;
	Value postvalue, getvalue;
	Reader jsonreader;
	StyledWriter jsonwriter;
	postvalue["path"] = path.c_str();
	data = jsonwriter.write(postvalue);
	string listurl = url;
	listurl += "api/fs/list";
	std::vector<std::string> returns;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, listurl.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, header.c_str());
		headers = curl_slist_append(headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &web::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &getdata);
		res = curl_easy_perform(curl);
	}
	curl_easy_cleanup(curl);
	if (!jsonreader.parse(getdata, getvalue)) {
		weblog << "JSON解析失败" << weblog.nl();
		returns.push_back("err");
		isok = 0;
		return returns;
	}
	else if (getvalue["code"].asInt() == 200) {
		// 访问data对象的content数组
		if (getvalue.isMember("data") && getvalue["data"].isObject() && getvalue["data"]["content"].isArray()) {
			const Json::Value& contentArray = getvalue["data"]["content"];
			for (const auto& item : contentArray) {
				if (item.isObject() && item.isMember("name")) {
					std::string name = item["name"].asString();
					list += name;
					list += "|";
				}
			}
		}
		returns = departfilename(list);
		return returns;
	}
	else {
		returns.push_back("err");
		isok = 0;
		return returns;
	}
}
web::web(string url_, string username, string password) {
	url = url_;
	isok = 1;
	gettoken(username, password);
	if (isok)
		getversion();
	if (isok)
		getdownloadurl();
#ifdef _DEBUG
	cout << versionfile << endl << raw_url << endl << token_value << endl;
#else
	weblog << weblog.pt() << versionfile << weblog.nl() << raw_url << weblog.nl() << token_value << weblog.nl();
#endif // _DEBUG
}
string web::token() {
	if (isok)
		return token_value;
	else return"err";
}
// 进度回调函数
int web::progress_callback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
	static const char* spinner = "|/-\\";
	static int spin_pos = 0,ms;
	static bool first_call = true, gotsized=false;
	static std::chrono::steady_clock::time_point last_update_time = std::chrono::steady_clock::now();
	double nowSize, totalSize,changedSize, changedSize_;
	string nowsize;
	static string totalsize, changedsize;
#ifdef _DEBUG
	ms = 100;
#else
	ms = 6000;
#endif
	const std::chrono::milliseconds update_interval(ms);
	if (first_call) {
		spin_pos = 0;
		first_call = false;
	}
	if (!gotsized) {
		if (dltotal / 1024 / 1024 / 1024 <= 1) {
			if ((dltotal / 1024 / 1024) <= 1) {
				if (dltotal / 1024 <= 1) {
					totalSize = dltotal;
				}
				else totalSize = dltotal / 1024;
			}
			else totalSize = dltotal / 1024 / 1024;
		}
		else totalSize = dltotal / 1024 / 1024 / 1024;
		totalsize = format("{:.1f}", totalSize);
		if (dltotal / 1024 / 1024 / 1024 <= 1) {
			if ((dltotal / 1024 / 1024) <= 1) {
				if (dltotal / 1024 <= 1) {
					totalsize += "Byte";
				}
				else
					totalsize += "KB";
			}
			else
				totalsize += "MB";
		}
		else
			totalsize += "GB";
		if (dltotal != 0)gotsized = 1;
	}
	auto now = std::chrono::steady_clock::now();
	if (now - last_update_time >= update_interval) {
		if (dlnow / 1024 / 1024 / 1024 <= 1) {
			if ((dlnow / 1024 / 1024) <= 1) {
				if (dlnow / 1024 <= 1) {
					nowSize = dlnow;
				}
				else nowSize = dlnow / 1024;
			}
			else nowSize = dlnow / 1024 / 1024;
		}
		else nowSize = dlnow / 1024 / 1024 / 1024;
		nowsize = format("{:.1f}", nowSize);
		if (dlnow / 1024 / 1024 / 1024 <= 1) {
			if ((dlnow / 1024 / 1024) <= 1) {
				if (dlnow / 1024 <= 1) {
					nowsize += "Byte";
				}
				else
					nowsize += "KB";
			}
			else nowsize += "MB";
		}
		else nowsize += "GB";
		changedSize_ = dlnow - last_file_size;
		if (changedSize_ != 0) {
			if (changedSize_ / 1024 / 1024 / 1024 / ms * 1000 <= 1) {
				if ((changedSize_ / 1024 / 1024 / ms * 1000) <= 1) {
					if (changedSize_ / 1024 / ms * 1000 <= 1) {
						changedSize = changedSize_;
					}
					else changedSize = changedSize_ / 1024 / ms * 1000;
				}
				else changedSize = changedSize_ / 1024 / 1024 / ms * 1000;
			}
			else changedSize = changedSize_ / 1024 / 1024 / 1024 / ms * 1000;
			changedsize = format("{:.1f}", changedSize);
			if (changedSize_ / 1024 / 1024 / 1024 / ms * 1000 <= 1) {
				if ((changedSize_ / 1024 / 1024 / ms * 1000) <= 1) {
					if (changedSize_ / 1024 / ms * 1000 <= 1) {
						changedsize += "Byte/s";
					}
					else
						changedsize += "KB/s";
				}
				else changedsize += "MB/s";
			}
			else changedsize += "GB/s";
		}
		int progress = static_cast<int>(dlnow * 100.0 / dltotal);
#ifdef _DEBUG
		int bar_width = 33;
		std::string bar(bar_width, ' ');
		int pos = bar_width * progress / 100;
		if (pos > 0) {
			bar.replace(0, pos, pos, '=');
			bar[pos] = spinner[spin_pos++ % 4];
			spin_pos %= 4;
			std::cout << "\r[" << bar << "]" << std::setw(3) << progress << "% " << nowsize << "/" << totalsize << " " << changedsize << std::flush;
		}
#else
		weblog << weblog.pt() << progress << "%" << nowsize << "/" << totalsize<<" " << changedsize << weblog.nl();
#endif // _DEBUG
		last_update_time = now;
		last_file_size = dlnow;
	}

	return 0;
}
// 用于写入下载数据的回调函数
size_t web::write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}
int web::downloadzip(const char* url, const char* filename) {
	CURL* curl;
	FILE* fp;
	CURLcode res;

	// 初始化libcurl
	curl = curl_easy_init();
	if (curl) {
		// 打开文件用于写入下载数据
		fp = fopen(filename, "wb");
		if (fp) {
			// 设置下载URL
			curl_easy_setopt(curl, CURLOPT_URL, url);
			// 不显示头信息
			curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
			// 设置写入回调函数
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			// 设置写入文件句柄
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			// 设置进度回调函数
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
			// 执行下载
			res = curl_easy_perform(curl);
			// 关闭文件
#ifdef _DEBUG
			std::cout << std::endl << "下载完成";
#else
			weblog << weblog.nl() << weblog.pt() << "下载完成";
#endif // _DEBUG

			fclose(fp);
		}
		else {
#ifdef _DEBUG
			std::cerr << "无法打开文件: " << filename << std::endl;
#else
			weblog << weblog.pt() << "无法打开文件: " << filename << weblog.nl();
#endif // _DEBUG
			isok = 0;
			return 1;
		}
		// 清理libcurl
		curl_easy_cleanup(curl);
	}
	else {
#ifdef _DEBUG
		std::cerr << "无法初始化libcurl" << std::endl;
#else
		weblog << weblog.pt() << "无法初始化libcurl" << weblog.nl();
#endif// _DEBUG
		isok = 0;
		return 1;
	}

	if (res != CURLE_OK) {
#ifdef _DEBUG
		std::cerr << "下载失败: " << curl_easy_strerror(res) << std::endl;
#else
		weblog << "下载失败: " << curl_easy_strerror(res) << weblog.nl();

#endif // _DEBUG
		isok = 0;
		return 1;
	}
	return 0;
}
bool web::begindownload(string filename)
{
#ifdef _DEBUG
	cout << "begin download" << endl;
#else
	weblog << "begin download" << weblog.nl();
#endif // _DEBUG
	downloadzip(raw_url.c_str(), filename.c_str());
#ifdef _DEBUG
	cout << "download ok" << endl;
#else
	weblog << "download ok" << weblog.nl();
#endif // _DEBUG
	return 0;
}
string web::gettoken(string username, string password) {
	CURL* curl;
	CURLcode res;
	string getdata, loginurl = url;
	Reader jsonreader;
	Value jsonvalue;
	string token_;
	string data = "{\n    \"username\": \"";
	data += username;
	data += "\",\n    \"password\": \"";
	data += password;
	data += "\"\n}";
	loginurl += "api/auth/login/";
	weblog << loginurl << weblog.nl();
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, loginurl.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &web::WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &getdata);
		res = curl_easy_perform(curl);
	}
	curl_easy_cleanup(curl);
	weblog << getdata << weblog.nl();
	if (!jsonreader.parse(getdata, jsonvalue)) {
#ifdef _DEBUG
		cout << "JSON解析失败" << endl;
#else
		weblog << "JSON解析失败" << weblog.nl();

#endif // _DEBUG
		isok = 0;
		return "err";
	}
	else if (jsonvalue["code"].asInt() == 200) {
		token_ = jsonvalue["data"]["token"].asString();
		header = "Authorization: ";
		token_value = token_;
		header += token_value;
		return token_;
	}
	else {
		isok = 0;
		return "err";
	}
}
size_t web::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t totalSize = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}
std::string web::findMaxVersion(const std::vector<std::string>& versions) {
	std::vector<std::string> numericParts;

	// 提取所有版本号的数字部分
	for (const auto& version : versions) {
		std::string numericPart;
		for (char c : version) {
			if (std::isdigit(c)) {
				numericPart += c;
			}
			else if (!numericPart.empty() && c == '.') {
				numericPart += '.';
			}
		}
		numericParts.push_back(numericPart);
	}

	// 找到最大的数字部分
	if (numericParts.empty())return "";
	std::sort(numericParts.begin(), numericParts.end(), std::greater<>());
	std::string maxNumericPart = removeAfterpoint(numericParts.front());

	// 在原始版本号中查找含有最大数字部分的版本号
	for (const auto& version : versions) {
		if (version.find(maxNumericPart) != std::string::npos) {
			return version;
		}
	}
	isok = 0;
	return ""; // 未找到匹配的版本号
}