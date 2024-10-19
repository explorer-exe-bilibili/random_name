//#define ZLIB_WINAPI
#include "unzip.h"

#include <iostream>
#include <vector>
#include <Shlwapi.h>
#include<minizip/unzip.h>
using namespace std;

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "minizip.lib")

string unzip::upgraderfile = "";
Log mlog("upgrade.log", 1);
// ���ַ����ڵ�old_value�滻��new_value
std::string& unzip::replace_all(std::string& str, const std::string& old_value, const std::string& new_value)
{
	while (true)
	{
		std::string::size_type pos(0);
		if ((pos = str.find(old_value)) != std::string::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

// �����༶Ŀ¼
bool unzip::CreatedMultipleDirectory(const std::string& direct)
{
	std::string Directoryname = direct;
	if (Directoryname[Directoryname.length() - 1] != '\\')
	{
		Directoryname.append(1, '\\');
	}
	std::vector< std::string> vpath;
	std::string strtemp;
	BOOL  bSuccess = FALSE;
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
		bSuccess = CreateDirectoryA(vIter->c_str(), NULL) ? TRUE : FALSE;
	}
	return bSuccess;
}

/*
* �������� : �ݹ��ѹ�ļ�Ŀ¼
* ��    ע : strFilePath ѹ����·��
*      strTempPath ��ѹ��
*/
void unzip::UnzipFile(const std::string& strFilePath, const std::string& strTempPath)
{
	int nReturnValue;
	string tempFilePath;
	string srcFilePath(strFilePath);
	string destFilePath;

	// ��zip�ļ�
	unzFile unzfile = unzOpen(srcFilePath.c_str());
	if (unzfile == NULL)
	{
		return;
	}

	// ��ȡzip�ļ�����Ϣ
	unz_global_info* pGlobalInfo = new unz_global_info;
	nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
	if (nReturnValue != UNZ_OK)
	{
#ifdef _DEBUG
		cout << "ѹ����: " << pGlobalInfo->number_entry << endl;
#else
		mlog << "ѹ����: " << pGlobalInfo->number_entry << mlog.nl();

#endif // DEBUG

		return;
	}

	// ����zip�ļ�
	unz_file_info* pFileInfo = new unz_file_info;
	char szZipFName[MAX_PATH] = { 0 };
	char szExtraName[MAX_PATH] = { 0 };
	char szCommName[MAX_PATH] = { 0 };

	// ��Ŵ�zip�н����������ڲ��ļ���
	for (unsigned int i = 0; i < pGlobalInfo->number_entry; i++)
	{
		// �����õ�zip�е��ļ���Ϣ
		nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH, szExtraName, MAX_PATH, szCommName, MAX_PATH);
		if (nReturnValue != UNZ_OK)
			return;
#ifdef _DEBUG
		cout << "��ѹ�ļ���: " << szZipFName;
#else
		mlog << "��ѹ�ļ���: " << szZipFName<<mlog.nl();
#endif // DEBUG

		string strZipFName = szZipFName;
		{
			if (strZipFName == "upgrade.bat" || strZipFName == "upgrade_.exe") {
				upgraderfile = strZipFName;
			}
			if (strZipFName == "upgrade.exe") {
				strZipFName = "upgrade_temp.exe";
			}
		}
		// �����Ŀ¼��ִ�д����ݹ�Ŀ¼��
		if (pFileInfo->external_fa == FILE_ATTRIBUTE_DIRECTORY || (strZipFName.rfind('/') == strZipFName.length() - 1))
		{
			destFilePath = strTempPath + "//" + szZipFName;
			CreateDirectoryA(destFilePath.c_str(), NULL);
		}

		// ������ļ����ѹ��������
		else
		{
			// �����ļ� ��������·��
			string strFullFilePath;
			tempFilePath = strTempPath + "/" + strZipFName.c_str();
			strFullFilePath = tempFilePath;
			bool fileusing = 0;
			if (tempFilePath == "./jsoncpp.dll" || tempFilePath == "./libcurl.dll" || tempFilePath == "./zlib1.dll" || tempFilePath == "./zlibwapi.dll")fileusing = 1;
			int nPos = tempFilePath.rfind("/");
			int nPosRev = tempFilePath.rfind("\\");
			if (nPosRev == string::npos && nPos == string::npos)
				continue;

			size_t nSplitPos = nPos > nPosRev ? nPos : nPosRev;
			destFilePath = tempFilePath.substr(0, nSplitPos + 1);

			if (!PathIsDirectoryA(destFilePath.c_str()))
			{
				// ��·����ʽͳһ
				destFilePath = replace_all(destFilePath, "/", "\\");
				// �����༶Ŀ¼
				int bRet = CreatedMultipleDirectory(destFilePath);
			}
			strFullFilePath = replace_all(strFullFilePath, "/", "\\");

			HANDLE hFile = 0;
			if (!fileusing)
				hFile = CreateFileA(strFullFilePath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				return;
			}

			// ���ļ�
			nReturnValue = unzOpenCurrentFile(unzfile);
			if (nReturnValue != UNZ_OK)
			{
				CloseHandle(hFile);
				return;
			}

			// ��ȡ�ļ�
			uLong BUFFER_SIZE = pFileInfo->uncompressed_size;;
			void* szReadBuffer = NULL;
			szReadBuffer = (char*)malloc(BUFFER_SIZE);
			if (NULL == szReadBuffer)
			{
				break;
			}
			if (!fileusing) {
				fileusing = 0;
				while (TRUE)
				{
					memset(szReadBuffer, 0, BUFFER_SIZE);
					int nReadFileSize = 0;

					nReadFileSize = unzReadCurrentFile(unzfile, szReadBuffer, BUFFER_SIZE);

					// ��ȡ�ļ�ʧ��
					if (nReadFileSize < 0)
					{
						unzCloseCurrentFile(unzfile);
						CloseHandle(hFile);
						return;
					}
					// ��ȡ�ļ����
					else if (nReadFileSize == 0)
					{
						unzCloseCurrentFile(unzfile);
						CloseHandle(hFile);
						break;
					}
					// д���ȡ������
					else
					{
						DWORD dWrite = 0;
						BOOL bWriteSuccessed = WriteFile(hFile, szReadBuffer, BUFFER_SIZE, &dWrite, NULL);
						if (!bWriteSuccessed)
						{
							unzCloseCurrentFile(unzfile);
							CloseHandle(hFile);
							break;
						}
					}
				}
				free(szReadBuffer);
			}
		}
		unzGoToNextFile(unzfile);
	}

	delete pFileInfo;
	delete pGlobalInfo;

	// �ر�
	if (unzfile)
	{
		unzClose(unzfile);
	}
}
