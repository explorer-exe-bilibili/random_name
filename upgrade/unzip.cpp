#include "unzip.h"

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "log.h"

#include <minizip/unzip.h>

using namespace std;
namespace fs= std::filesystem;

static bool CreatedMultipleDirectory(const std::string& direct);

unzip::unzip(const std::string& zipFile) : zipFilePath(zipFile) {
    Log<<Level::Info<<"Unzip file: "<<zipFilePath<<op::endl;
    if (!fs::exists(zipFilePath)) {
        Log<<Level::Error<<"Zip file does not exist: "<<zipFilePath<<op::endl;
    }
    if (!fs::is_regular_file(zipFilePath)) {
        Log<<Level::Error<<"Not a regular file: "<<zipFilePath<<op::endl;
    }
    if (!fs::is_empty(zipFilePath)) {
        Log<<Level::Error<<"Zip file is not empty: "<<zipFilePath<<op::endl;
    }
    if (!fs::is_directory(fs::path(zipFilePath).parent_path())) {
        Log<<Level::Error<<"Parent directory does not exist: "<<fs::path(zipFilePath).parent_path().string()<<op::endl;
    }
    // 打开zip文件
    zipFileHandle = unzOpen(zipFilePath.c_str());
    if (zipFileHandle == nullptr) {
        Log<<Level::Error<<"Failed to open zip file: "<<zipFilePath<<op::endl;
    }
}

bool unzip::extract(const std::string& destDir){
    // 检查目标目录是否存在，如果不存在则创建
    if (!fs::exists(destDir)) {
        Log<<Level::Info<<"Creating destination directory: "<<destDir<<op::endl;
        if (!CreatedMultipleDirectory(destDir)) {
            Log<<Level::Error<<"Failed to create destination directory: "<<destDir<<op::endl;
            return false;
        }
    }
    Log<<Level::Info<<"Extracting files to: "<<destDir<<op::endl;
    // 获取zip文件信息
    if (zipFileHandle == nullptr) {
        Log<<Level::Error<<"Zip file handle is null, cannot extract files.";
        return false;
    }
    unz_global_info* globalInfo= new unz_global_info;
    if (unzGetGlobalInfo(zipFileHandle, globalInfo) != UNZ_OK) {
        Log<<Level::Error<<"Failed to get global info from zip file: "<<zipFilePath;
        return false;
    }
    Log<<Level::Info<<"Number of entries in zip file: "<<globalInfo->number_entry<<op::endl;

    // 解析zip文件
    unz_file_info* pFileInfo= new unz_file_info;
    char szZipFName[512]={0};
    char szExtraName[512]={0};
    char szCommName[512]={0};

    // 存放文件名
    for(unsigned int i=0;i<globalInfo->number_entry;i++){
        //解析得到zip中的文件信息
        int result = unzGetCurrentFileInfo(zipFileHandle,pFileInfo,szZipFName,512,szExtraName,512,szCommName,512);
        if(result!=UNZ_OK)return false;
        Log<<"正在解压："<< szZipFName<<op::endl;

        string strZipFName(szZipFName);
        if(strZipFName=="upgrade.bat"||strZipFName=="upgrade_.exe"||strZipFName=="upgrade.sh")upgraderFile=strZipFName;
        if(strZipFName=="upgrade.exe")strZipFName="upgrade_temp.exe";
        if(pFileInfo->external_fa==0x10||
            (strZipFName.rfind('/')==strZipFName.length()-1)||
            (strZipFName.rfind('\\')==strZipFName.length()-1)){
            string t=destDir+"/"+szZipFName;
            fs::create_directory(t);
        }
        else{
            //如果是文件则解压并创建
            // 创建文件 保存路径
            string fullPath;
            string t=destDir+"/"+szZipFName;
            fullPath=t;
            size_t lastSlashPos = fullPath.find_last_of("/\\");
            if (lastSlashPos == std::string::npos) continue;
            size_t nSplitPos = lastSlashPos;
            fullPath = fullPath.substr(nSplitPos + 1);

            fs::path filePath(fullPath);
            fs::create_directories(filePath.parent_path());

            ofstream outFile(filePath, std::ios::binary);
            if (!outFile) {
                Log<<Level::Error<<"Failed to create file: "<<filePath.string()<<op::endl;
                continue;
            }

            // 解压文件
            if (unzOpenCurrentFile(zipFileHandle) != UNZ_OK) {
                Log<<Level::Error<<"Failed to open file in zip: "<<szZipFName<<op::endl;
                continue;
            }

            char buffer[8192];
            int bytesRead;
            while ((bytesRead = unzReadCurrentFile(zipFileHandle, buffer, sizeof(buffer))) > 0) {
                outFile.write(buffer, bytesRead);
                for(auto& c:buffer)c=0;
            }

            unzCloseCurrentFile(zipFileHandle);
            Log<<Level::Info<<"Extracted file: "<<filePath.string()<<op::endl;
        }
    }
    delete globalInfo;
    delete pFileInfo;
    return true;
}

unzip::~unzip() {
    if (zipFileHandle != nullptr) {
        unzClose(zipFileHandle);
    }
}


// 创建多级目录
static bool CreatedMultipleDirectory(const std::string& direct)
{
	std::string Directoryname = direct;
	if (Directoryname[Directoryname.length() - 1] != '\\')
	{
		Directoryname.append(1, '\\');
	}
	std::vector< std::string> vpath;
	std::string strtemp;
	bool  bSuccess = false;
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
        bSuccess = fs::create_directories(*vIter);
	}
	return bSuccess;
}