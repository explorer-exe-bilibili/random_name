#include "unzip.h"

#include <string>
#include <cstring>
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
    std::string destFilePath;
    std::string tempFilePath;
	for (unsigned int i = 0; i < globalInfo->number_entry; i++)
	{
		// 解析得到zip中的文件信息
		int nReturnValue = unzGetCurrentFileInfo(zipFileHandle, pFileInfo, szZipFName, 512, szExtraName, 512, szCommName, 512);
		if (nReturnValue != UNZ_OK)
			return false;
		Log << Level::Info << "解压文件名: " << szZipFName;

		string strZipFName = szZipFName;
		{
			if (strZipFName == "upgrade.bat" || strZipFName == "upgrade_.exe") {
				upgraderFile = strZipFName;
			}
			if (strZipFName == "upgrade.exe") {
				strZipFName = "upgrade_temp.exe";
			}
		}
		// 如果是目录则执行创建递归目录名
		if (pFileInfo->external_fa == 0x10 || (strZipFName.rfind('/') == strZipFName.length() - 1))
		{
			destFilePath = destDir + "//" + szZipFName;
			fs::create_directory(destFilePath);
		}

		// 如果是文件则解压缩并创建
		else
		{
			// 创建文件 保存完整路径
			string strFullFilePath;
			tempFilePath = destDir + "/" + strZipFName.c_str();
			strFullFilePath = tempFilePath;
			bool fileusing = 0;
			if (tempFilePath == "./jsoncpp.dll" || tempFilePath == "./libcurl.dll" || tempFilePath == "./zlib1.dll" || tempFilePath == "./zlibwapi.dll")fileusing = 1;
			int nPos = tempFilePath.rfind("/");
			int nPosRev = tempFilePath.rfind("\\");
			if (nPosRev == string::npos && nPos == string::npos)
				continue;

			size_t nSplitPos = nPos > nPosRev ? nPos : nPosRev;
			destFilePath = tempFilePath.substr(0, nSplitPos + 1);

			if (!fs::is_directory(destFilePath)){
				// 创建多级目录
				int bRet = CreatedMultipleDirectory(destFilePath);
			}
            // 创建文件
            FILE* hFile = fopen(strFullFilePath.c_str(), "wb");
            if (hFile == NULL){
                Log << Level::Error << "Failed to create file: " << strFullFilePath << op::endl;
                unzCloseCurrentFile(zipFileHandle);
                return false;
            }

			// 打开文件
			int nReturnValue = unzOpenCurrentFile(zipFileHandle);
			if (nReturnValue != UNZ_OK)
			{
				fclose(hFile);
				return false;
			}

			// 读取文件
			uLong BUFFER_SIZE = pFileInfo->uncompressed_size;;
			void* szReadBuffer = NULL;
			szReadBuffer = (char*)malloc(BUFFER_SIZE);
			if (NULL == szReadBuffer)
			{
				break;
			}
			if (!fileusing) {
				fileusing = 0;
				while (1)
				{
					memset(szReadBuffer, 0, BUFFER_SIZE);
					int nReadFileSize = 0;

					nReadFileSize = unzReadCurrentFile(zipFileHandle, szReadBuffer, BUFFER_SIZE);

					// 读取文件失败
					if (nReadFileSize < 0)
					{
						unzCloseCurrentFile(zipFileHandle);
						fclose(hFile);
						return false;
					}
					// 读取文件完毕
					else if (nReadFileSize == 0)
					{
						unzCloseCurrentFile(zipFileHandle);
						fclose(hFile);
						break;
					}
					// 写入读取的内容
					else
					{
                        size_t bWriteSuccessed = fwrite(szReadBuffer, 1, nReadFileSize, hFile);
                        if (bWriteSuccessed != nReadFileSize)
						{
							unzCloseCurrentFile(zipFileHandle);
							fclose(hFile);
							break;
						}
					}
				}
				free(szReadBuffer);
			}
		}
		unzGoToNextFile(zipFileHandle);
	}

	delete pFileInfo;
	delete globalInfo;

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