//upgrade.cpp
#include <iostream>
#include <stdio.h>
#include"unzip.h"
#include"Log.h"
#include"web.h"
#include"task.h"
#include"local.h"
#include<filesystem>
#include <Shlwapi.h>

using namespace std;

Log mlog2("upgrade.log", 0);
bool upgrade(string url) {
#ifdef _DEBUG
	web get(url,"apiuser", "12345678");
	//web get("http://52.184.81.18:35244/", "apiuser", "12345678");
#else
	web get(url, "apiuser", "12345678");
#endif // _DEBUG
	if (get.isok) {
		if (!local::isupdated(web::version())) {
#ifdef _M_IX86
			get.begindownload("download(x86).zip");
#else
			get.begindownload("download.zip");
#endif // _M_IX86
			while (task::processExists(L"ramdon name.exe")) {
				Sleep(1000);
			}
			mlog2 << mlog2.pt() << "begin" << mlog2.nl();
#ifdef _M_IX86
			std::string srcFilePath = ".\\download(x86).zip";
#else
			std::string srcFilePath = ".\\download.zip";
#endif // _M_IX86

			std::string tempdir = ".";
			// 如果传入目录不存在则创建
			if (!::PathFileExistsA(tempdir.c_str())) {
				unzip::CreatedMultipleDirectory(tempdir);
				mlog2 << mlog2.pt() << "创建了临时目录" << mlog2.nl();
			}
			// 调用解压函数
			unzip::UnzipFile(srcFilePath, tempdir);
			if (unzip::upgraderfile != "") {
				ShellExecuteA(NULL, "open", unzip::upgraderfile.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
			if (std::filesystem::exists(srcFilePath))
				remove(srcFilePath.c_str());
			mlog2 << mlog2.pt() << "end" << mlog2.nl();
			return 1;
		}
		else {
			mlog2 << "已经是最高版本\n";
			return 0;
		}
	}
}
int main()
{
	mlog2 << "upgrade start\n";

	if (!upgrade("http://8.137.121.212:55244/")) {
		if (!upgrade("http://39.104.26.184:55244/")) {
			if (!upgrade("http://120.76.205.26:55244/"))
				mlog2 << "服务器不在线，请稍后再试";
		}
	}
	return 0;
}