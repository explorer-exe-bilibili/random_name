#include "web.h"

#include "log.h"
#include "version.h"
#include <regex>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>

using namespace nlohmann;
using namespace std;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t totalSize = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

static int getFileVersion(const std::string& input){
    std::regex pattern(R"(v\d+\.\d+\.\d+(-\w+)?)");

    std::sregex_iterator it(input.begin(), input.end(), pattern);
    std::sregex_iterator end;
    int result = 0;

    for (; it != end; ++it) {
        std::string match = it->str();
        std::smatch versionMatch;
        if (std::regex_search(match, versionMatch, std::regex(R"((\d+)\.(\d+)\.(\d+))")) && versionMatch.size() == 4) {
            int major = std::stoi(versionMatch[1].str());
            int minor = std::stoi(versionMatch[2].str());
            int patch = std::stoi(versionMatch[3].str());
            result = major * 10000 + minor * 100 + patch;
        }
    }
    return result;
}

alist_api::alist_api(const std::string& baseUrl, const std::string& username, const std::string& password)
    : m_baseUrl(baseUrl) {
    m_headers = curl_slist_append(nullptr, "Content-Type: application/json");
    m_headers = curl_slist_append(m_headers, "User-Agent: Apifox/1.0.0 (https://apifox.com)");

    m_curl = curl_easy_init();
    if(!m_curl) {
        Log << Level::Error << "Failed to initialize CURL." << op::endl;
        return;
    }
    CURLcode res;
    std::string token_;
    std::string data = "{\n    \"username\": \"";
    data += username;
    data += "\",\n    \"password\": \"";
    data += password;
    data += "\"\n}";
    std::string url = m_baseUrl + "api/auth/login";
    Log<<Level::Info<<"Alist API URL: "<<url<<" data: "<<data<<op::endl;
    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &token_);
    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_curl, CURLOPT_DEFAULT_PROTOCOL, "http");
    res = curl_easy_perform(m_curl);
    if (res != CURLE_OK) {
        Log << Level::Error << "CURL error: " << curl_easy_strerror(res) << op::endl;
    }
    else {
        Log << Level::Info << "CURL response: " << token_ << op::endl;
        std::regex token_regex(R"("token":\s*"([^"]+))");
        std::smatch match;
        if (std::regex_search(token_, match, token_regex) && match.size() > 1) {
            m_key = match[1].str();
            string header="Authorization: "+m_key;
            curl_slist_append(m_headers, header.c_str());
            Log << Level::Info << "Token extracted: " << m_key << op::endl;
        } else {
            Log << Level::Error << "Failed to extract token from response." << op::endl;
        }
    }
}

alist_api::~alist_api() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
        m_curl = nullptr;
    }
    if(m_headers){
        curl_slist_free_all(m_headers);
        m_headers = nullptr;
    }
}

// 用于写入下载数据的回调函数
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
	static const char* spinner = "|/-\\";
	static int spin_pos = 0,ms;
	static bool first_call = true, gotsized=false;
	static std::chrono::steady_clock::time_point last_update_time = std::chrono::steady_clock::now();
	double nowSize, totalSize,changedSize, changedSize_;
    static curl_off_t last_file_size = 0;
	string nowsize;
	static string totalsize, changedsize;
	ms = 2000;
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
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(1) << totalSize;
		totalsize = oss.str();
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
		std::ostringstream oss2;
		oss2 << std::fixed << std::setprecision(1) << nowSize;
		nowsize = oss2.str();
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
			std::ostringstream oss3;
			oss3 << std::fixed << std::setprecision(1) << changedSize;
			changedsize = oss3.str();
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
		Log << op::time << progress << "%" << nowsize << "/" << totalsize<<" " << changedsize << op::endl;
		last_update_time = now;
		last_file_size = dlnow;
	}

	return 0;
}

bool alist_api::downloadFile(const std::string& url, const std::string& destPath) {
	CURL* curl;
	FILE* fp;
	CURLcode res;

	// 初始化libcurl
	curl = curl_easy_init();
	if (curl) {
		// 打开文件用于写入下载数据
		fp = fopen(destPath.c_str(), "wb");
		if (fp) {
			// 设置下载URL
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			// 不显示头信息
			curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
			// 设置写入回调函数
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
			// 设置写入文件句柄
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			// 设置进度回调函数
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
			curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
			// 执行下载
			res = curl_easy_perform(curl);
			// 关闭文件
			Log << Level::Info << "download completed" << op::endl;

			fclose(fp);
		}
		else {
		Log << Level::Error << "failed to open file: " << destPath << op::endl;
			return false;
		}
		// 清理libcurl
		curl_easy_cleanup(curl);
	}
	else {
		Log << Level::Error << "failed to initialize libcurl" << op::endl;
		return false;
	}

	if (res != CURLE_OK) {
		Log << Level::Error << "failed to download: " << curl_easy_strerror(res) << op::endl;
		return false;
	}
	return true;
}

std::string alist_api::getFileUrl() {
    if (m_key.empty()) {
        Log << Level::Error << "Token not set." << op::endl;
        return "";
    }
    std::vector<std::string> fileList;
    std::map<int, std::string> versionList;
    listFiles("/", fileList);
    listFiles("/", fileList);
    for (const auto& f : fileList) {
        int version = getFileVersion(f);
        Log << Level::Info << "File found: " << f << " File version: " << version << op::endl;
        Log << Level::Info << "File found: " << f << " File version: " << version << op::endl;
        if (version >= 0) {
            versionList[version] = f;
        }
    }
    if (versionList.empty()) {
        Log << Level::Warn << "No valid file versions found." << op::endl;
        return "";
    }
    if(versionList.rbegin()->first<=VERSION_NUMBER) {
        Log << Level::Info << "No new version available. current version: " << VERSION_NUMBER << op::endl;
        return "new_version_not_found";
        Log << Level::Info << "No new version available. current version: " << VERSION_NUMBER << op::endl;
        return "new_version_not_found";
    }
    CURLcode res;
    std::string geturl = m_baseUrl + "api/fs/get";
    std::string getdata;
    json post,get;
    post["path"]=versionList.rbegin()->second+"/"+"random_name_"+ARCHITECTURE+"_"+PLATFORM_NAME+".zip";
    post["path"]=versionList.rbegin()->second+"/"+"random_name_"+ARCHITECTURE+"_"+PLATFORM_NAME+".zip";
    string data=post.dump();
    if(m_curl){
        Log<<"getFileUrl() url:"<<geturl<<" data: "<<data<<op::endl;
        curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(m_curl, CURLOPT_URL, geturl.c_str());
		curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(m_curl, CURLOPT_DEFAULT_PROTOCOL, "http");
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);
		curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
		curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &getdata);
		res = curl_easy_perform(m_curl);
    }
    if (res != CURLE_OK) {
        Log << Level::Error << "CURL error: " << curl_easy_strerror(res) << op::endl;
        return "";
    }
    Log << Level::Info << "CURL response: " << getdata << op::endl;
    try {
        get = json::parse(getdata);
    } catch (const json::parse_error& e) {
        Log << Level::Error << "JSON parse error: " << e.what() << op::endl;
        return "";
    }
    if (get["code"] != 200) {
        Log << Level::Error << "API error: " << get["message"].get<std::string>() << " (code: " << (int)get["code"] << ")" << op::endl;
        return "";
    }
    std::string url = get["data"]["raw_url"].get<std::string>();
    if (url.empty()) {
        Log << Level::Error << "No URL found in API response." << op::endl;
        return "";
    }
    Log << Level::Info << "File URL: " << url << op::endl;
    return url;
}

bool alist_api::listFiles(const std::string& dirPath, std::vector<std::string>& fileList) {
    if (m_curl == nullptr || m_key.empty()) {
        Log << Level::Error << "CURL not initialized or token not set." << op::endl;
        return false;
    }
    CURLcode res;
    std::string listUrl = m_baseUrl + "api/fs/list";
    std::string listData;
    json post, response;
    post["path"] = dirPath;
    std::string data = post.dump();
    fileList.clear();
    Log << Level::Info << "Listing files in directory: " << dirPath << op::endl;
    if (m_curl) {
        Log<<"listFiles() url:"<<listUrl<<" data: "<<data<<op::endl;
        curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(m_curl, CURLOPT_URL, listUrl.c_str());
        curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(m_curl, CURLOPT_DEFAULT_PROTOCOL, "http");
        curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
        curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &listData);
        res = curl_easy_perform(m_curl);
    }
    if (res != CURLE_OK) {
        Log << Level::Error << "CURL error: " << curl_easy_strerror(res) << op::endl;
        return false;
    }
    Log << Level::Info << "CURL response: " << listData << op::endl;
    try {
        response = json::parse(listData);
    } catch (const json::parse_error& e) {
        Log << Level::Error << "JSON parse error: " << e.what() << op::endl;
        return false;
    }
    if (response["code"] != 200) {
        Log << Level::Error << "API error: " << response["message"].get<std::string>() << " (code: " << (int)response["code"] << ")" << op::endl;
        return false;
    }
    if (!response["data"].contains("content")) {
        Log << Level::Error << "No 'content' key found in API response." << op::endl;
        return false;
    }
    for (const auto& item : response["data"]["content"]) {
        if (item.contains("name")) {
            fileList.push_back(item["name"].get<std::string>());
        }
    }
    return true;
}