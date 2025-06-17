#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>

class alist_api{
public:
    alist_api(const std::string& baseUrl, const std::string& username, const std::string& password);
    ~alist_api();

    bool downloadFile(const std::string& url, const std::string& destPath);
    std::string getFileUrl();
    bool listFiles(const std::string& dirPath, std::vector<std::string>& fileList);
    bool isConnectAble(){return !m_key.empty();}
private:
    std::string m_baseUrl;
    std::string m_key;
    CURL* m_curl;
    struct curl_slist* m_headers;
};