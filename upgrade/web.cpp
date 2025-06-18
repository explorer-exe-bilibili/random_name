#include "web.h"

#include "log.h"
#include "version.h"
#include <regex>
#include <nlohmann/json.hpp>

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

int progress_callback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
    // 用于显示进度条，速度，已下载大小，总大小等信息
    static double lastSize = 0;
    double change = dlnow - lastSize;
    lastSize = dlnow;
    if (dltotal <= 0) {
        Log << Level::Warn << "下载总大小未知，无法计算进度。" << op::endl;
        return 0; // 返回0表示继续下载
    }
    if (dlnow < 0 || dltotal < 0) {
        Log << Level::Error << "下载进度异常: 已下载大小或总大小为负数。" << op::endl;
        return 0; // 返回0表示继续下载
    }
    // 计算下载进度百分比
    double progress = (dlnow / dltotal) * 100.0;
    // 自适应转换单位
    std::string unitn = "bytes";
    std::string unitt = "bytes";
    std::string units = "bytes/s";
    if (dlnow > 1024) {
        dlnow /= 1024;
        unitn = "KB";
    }
    if (dltotal > 1024) {
        dltotal /= 1024;
        unitt = "KB";
    }
    if (dlnow > 1024) {
        dlnow /= 1024;
        unitn = "MB";
    }
    if (dltotal > 1024) {
        dltotal /= 1024;
        unitt = "MB";
    }
    if (dlnow > 1024) {
        dlnow /= 1024;
        unitn = "GB";
    }
    if (dltotal > 1024) {
        dltotal /= 1024;
        unitt = "GB";
    }
    if(change > 1024) {
        change /= 1024;
        units = "KB/s";
    }
    if(change > 1024) {
        change /= 1024;
        units = "MB/s";
    }
    if(change > 1024) {
        change /= 1024;
        units = "GB/s";
    }
    // 输出下载进度信息
    Log << Level::Info << "下载速度: " << change << units
     << progress << "%, 已下载: " << dlnow << " " << unitn << ", 总大小: " << dltotal << " " << unitt << op::endl;
    return 0; // 返回0表示继续下载
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
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
			// 执行下载
			res = curl_easy_perform(curl);
			// 关闭文件
			Log << Level::Info << "下载完成" << op::endl;

			fclose(fp);
		}
		else {
		Log << Level::Error << "无法打开文件: " << destPath << op::endl;
			return 1;
		}
		// 清理libcurl
		curl_easy_cleanup(curl);
	}
	else {
		Log << Level::Error << "无法初始化libcurl" << op::endl;
		return 1;
	}

	if (res != CURLE_OK) {
		Log << Level::Error << "下载失败: " << curl_easy_strerror(res) << op::endl;
		return 1;
	}
	return 0;
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