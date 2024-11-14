#include "set-json.h"
#include "nlohmann/json.hpp"
#include "log.h"
#include "config.h"
#include"configitem.h"
#include "item.h"

using namespace std;
using namespace nlohmann;

json set2::rollback(const string& jsonpath) {
	setlocale(LC_ALL, "zh-CN.UTF-8");
	Log slog("files\\log\\set-json.log", 0);
	slog << "try to rollback setting page" << endl;
	// 创建JSON数据a
	json j;
	json p;
	json i;
	p[TITLE] =G2U(L"图片");
	i[NAME] =G2U(L"卡池1图片"); i[CONFIGNAME] =G2U(L"over1"); i[FILECHOOSE] =G2U(L"选择卡池1图片");
	i[FILETYPE] =G2U(L"picture"); i[LIMIT] = ISBITMAP | ISFILE; i[NUMBER] = 1; i[BITMAPC] = 0;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"卡池2图片"); i[CONFIGNAME] =G2U(L"over2"); i[FILECHOOSE] =G2U(L"选择卡池2图片");
	i[FILETYPE] =G2U(L"picture"); i[LIMIT] = ISBITMAP | ISFILE; i[NUMBER] = 2; i[BITMAPC] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"卡池3图片"); i[CONFIGNAME] =G2U(L"over3"); i[FILECHOOSE] =G2U(L"选择卡池3图片");
	i[FILETYPE] =G2U(L"picture"); i[LIMIT] = ISBITMAP | ISFILE; i[NUMBER] = 3; i[BITMAPC] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"卡池4图片"); i[CONFIGNAME] =G2U(L"over4"); i[FILECHOOSE] =G2U(L"选择卡池4图片");
	i[FILETYPE] =G2U(L"picture"); i[LIMIT] = ISBITMAP | ISFILE; i[NUMBER] = 4; i[BITMAPC] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"图片移动的速度"); i[CONFIGNAME] =G2U(L"first screen Picture speed"); i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 10000; i[MIN] = 0;
	i[OUTOFLIMIT] =G2U(L"输入一个0-10000之间的数字"); i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"关闭音乐"); i[CONFIGNAME] =G2U(L"off music"); i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();

	j["pages"].push_back(p); p.clear();
	p[TITLE] =G2U(L"名单与卡池");
	i[NAME] =G2U(L"卡池1名单"); i[CONFIGNAME] =G2U(L"namesfile1"); i[FILECHOOSE] =G2U(L"选择卡池1名单"); i[FILETYPE] =G2U(L"nameFile"); i[LIMIT] = ISFILE; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"卡池2名单"); i[CONFIGNAME] =G2U(L"namesfile2"); i[FILECHOOSE] =G2U(L"选择卡池2名单"); i[FILETYPE] =G2U(L"nameFile"); i[LIMIT] = ISFILE; i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"卡池3名单"); i[CONFIGNAME] =G2U(L"namesfile3"); i[FILECHOOSE] =G2U(L"选择卡池3名单"); i[FILETYPE] =G2U(L"nameFile"); i[LIMIT] = ISFILE; i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"卡池4名单"); i[CONFIGNAME] =G2U(L"namesfile4"); i[FILECHOOSE] =G2U(L"选择卡池4名单"); i[FILETYPE] =G2U(L"nameFile"); i[LIMIT] = ISFILE; i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"抽卡时名字的颜色"); i[CONFIGNAME] =G2U(L"name color"); i[ISCOLOR] = 1; i[NUMBER] = 5;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"抽卡时6星名字的颜色"); i[CONFIGNAME] =G2U(L"6 star name color"); i[ISCOLOR] = 1; i[NUMBER] = 6;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"抽背卡池"); i[CONFIGNAME] =G2U(L"special"); i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 4; i[MIN] = 0;
	i[OUTOFLIMIT] =G2U(L"输入一个0-4之间的数字（0表示禁用）"); i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"关闭音乐"); i[CONFIGNAME] =G2U(L"off music"); i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] =G2U(L"视频");
	i[NAME] =G2U(L"单发3星视频"); i[CONFIGNAME] =G2U(L"signal 3star video"); i[LIMIT] = ISFILE; i[FILECHOOSE] =G2U(L"选择单发3星视频"); i[FILETYPE] =G2U(L"video"); i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"单发4星视频"); i[CONFIGNAME] =G2U(L"signal 4star video"); i[LIMIT] = ISFILE; i[FILECHOOSE] =G2U(L"选择单发4星视频"); i[FILETYPE] =G2U(L"video"); i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"单发5星视频"); i[CONFIGNAME] =G2U(L"signal 5star video"); i[LIMIT] = ISFILE; i[FILECHOOSE] =G2U(L"选择单发5星视频"); i[FILETYPE] =G2U(L"video"); i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"十发4星视频"); i[CONFIGNAME] =G2U(L"group 4star video"); i[LIMIT] = ISFILE; i[FILECHOOSE] =G2U(L"选择十发4星视频"); i[FILETYPE] =G2U(L"video"); i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"十发5星视频"); i[CONFIGNAME] =G2U(L"group 5star video"); i[LIMIT] = ISFILE; i[FILECHOOSE] =G2U(L"选择十发5星视频"); i[FILETYPE] =G2U(L"video"); i[NUMBER] = 5;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"关闭视频"); i[CONFIGNAME] =G2U(L"off video"); i[ISSWITCH] = 1; i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"关闭音乐"); i[CONFIGNAME] =G2U(L"off music"); i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] =G2U(L"悬浮窗(重启生效)");
	i[NAME] =G2U(L"悬浮窗"); i[CONFIGNAME] =G2U(L"open float window"); i[ISSWITCH] = 1; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"初始x坐标"); i[CONFIGNAME] =G2U(L"float window x"); i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] =G2U(L"大小不能大于屏幕"); i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"初始y坐标"); i[CONFIGNAME] =G2U(L"float window y"); i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] =G2U(L"大小不能大于屏幕"); i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"宽度"); i[CONFIGNAME] =G2U(L"float window width"); i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] =G2U(L"大小不能大于屏幕"); i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"高度"); i[CONFIGNAME] =G2U(L"float window height"); i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] =G2U(L"大小不能大于屏幕"); i[NUMBER] = 5;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"滑动系数"); i[CONFIGNAME] =G2U(L"float window Mu"); i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = 1; i[MIN] = 0;
	i[OUTOFLIMIT] =G2U(L"大小不能超过1或小于0"); i[NUMBER] = 6;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"悬浮窗图片"); i[CONFIGNAME] =G2U(L"float window picture"); i[FILECHOOSE] =G2U(L"选择悬浮窗图片");
	i[FILETYPE] =G2U(L"picture"); i[LIMIT] = ISBITMAP | REBOOT; i[NUMBER] = 11; i[BITMAPC] = FLOATPIC;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"关闭音乐"); i[CONFIGNAME] =G2U(L"off music"); i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] =G2U(L"杂项");
	i[NAME] =G2U(L"窗口模式"); i[CONFIGNAME] =G2U(L"window mode(not full screen)"); i[ISSWITCH] = 1; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"标题"); i[CONFIGNAME] =G2U(L"title name"); i[ISEDIT] = 1; i[LIMIT] = S_WINDOWTITLE; i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"设置页面不使用json文件"); i[CONFIGNAME] =G2U(L"cancel setting json file"); i[ISSWITCH] = 1; i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"字体兼容模式(出现字体错误再开)"); i[CONFIGNAME] =G2U(L"the new font is unsuit"); i[ISSWITCH] = 1; i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"关闭音乐"); i[CONFIGNAME] =G2U(L"off music"); i[ISSWITCH] = 1; i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"调试模式"); i[ISSWITCH] = 1; i[NUMBER] = 5; i[CONFIGNAME] =G2U(L"debug mode"); i[LIMIT] = REBOOT;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"省内存模式"); i[ISSWITCH] = 1; i[NUMBER] = 6; i[CONFIGNAME] =G2U(L"small memory"); i[LIMIT] = REBOOT;
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"关闭动画"); i[ISSWITCH] = 1; i[NUMBER] = 8; i[CONFIGNAME] =G2U(L"no smooth ui"); 
	p["item"].push_back(i); i.clear();
	i[NAME] =G2U(L"图片资源目录"); i[ISEDIT] = 1; i[NUMBER] = 7; i[CONFIGNAME] =G2U(L"ImageDirectory"); i[LIMIT] = REBOOT; i[ISDIR] = 1;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	// 写入JSON文件
	if (!config::getint(NOSETTINGFILE)) {
		std::ofstream file(jsonpath);
		if (file.is_open()) {
			try
			{
				slog << slog.pt() << "[INFO]begin to write json items";
				file << j.dump(4); // 将JSON数据写入文件，4表示缩进4个空格
				file.close();
				slog << "[INFO]JSON data written to" << jsonpath << endl;
			}
			catch (const std::exception& e)
			{
				slog << slog.pt() << "[ERROR]meet an error when writing json item (rollback):" << e.what() << endl;
				return j;
			}
		}
		else {
			slog << "[ERROR]Failed to open file for writing" << std::endl;
		}
	}
	return j;
}
// UTF8字符串转成GBK字符串
std::string set2::U2G(const std::string& utf8)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];//加1用于截断字符串 
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}
// GBK字符串转成json识别的UTF8字符串
std::string set2::G2U(const std::wstring& gbk)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, gbk.c_str(), -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[129];
	memset(pBuf, 0, 128 + 1);

	WideCharToMultiByte(CP_UTF8, 0, gbk.c_str(), -1, pBuf, 128, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;

	pBuf = NULL;

	return retStr;
}
