#include "unzip.h"
#include "web.h"
#include <string>
#include <vector>


int main(){
    Log.setFile("files/upgrade.log");
    Log.setAutoFlush(true);
    Log << Level::Info << "starting upgrade process" << op::endl;
    // 服务器列表
    std::vector<std::string> servers = {
        "https://alist.1class.top/",
        "https://alist.class1.dpdns.org/",
        "http://hn.lxhtt.cn:55246/",
        "http://45.207.156.82:15244/",
        "http://hn.lxhtt.cn:15244/"
    };
    
    bool downloadSuccess = false;
    
    // 遍历服务器列表尝试下载
    for(const auto& serverUrl : servers) {
        alist_api api(serverUrl, "apiuser", "password");
        if(api.isConnectAble()) {
            std::string downloadurl = api.getFileUrl();
            if(downloadurl.empty()) {
                Log << Level::Error << "Failed to get download link" << op::endl;
                continue;
            }
            else if(downloadurl=="new_version_not_found") {
                Log << Level::Info << "no newer version" << op::endl;
                return 0;
            }
            if(api.downloadFile(downloadurl, "package.zip")) {
                downloadSuccess = true;
                break;
            }
        }
    }
    
    if(!downloadSuccess) {
        return -1;
    }
    unzip Unzip("package.zip");
    Unzip.extract(".");
    return 0;
}