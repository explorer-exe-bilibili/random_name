#include "set-json.h"
#include "nlohmann/json.hpp"
#include "log.h"
#include "config.h"
#include"configitem.h"
#include "item.h"

using namespace std;
using namespace nlohmann;

json set2::rollback(string jsonpath) {
	Log slog("files\\log\\set-json.log", 0);
	slog << "try to rollback setting page" << endl;
	// ����JSON����
	json j;
	json p;
	json i;
	p[TITLE] = G2U("ͼƬ");
	i[NAME] = G2U("����1ͼƬ"); i[CONFIGNAME] = "over1"; i[FILECHOOSE] = G2U("ѡ�񿨳�1ͼƬ");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP | ISFILE; i[NUMBER] = 1; i[BITMAPC] = 0;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����2ͼƬ"); i[CONFIGNAME] = "over2"; i[FILECHOOSE] = G2U("ѡ�񿨳�2ͼƬ");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP | ISFILE; i[NUMBER] = 2; i[BITMAPC] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����3ͼƬ"); i[CONFIGNAME] = "over3"; i[FILECHOOSE] = G2U("ѡ�񿨳�3ͼƬ");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP | ISFILE; i[NUMBER] = 3; i[BITMAPC] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����4ͼƬ"); i[CONFIGNAME] = "over4"; i[FILECHOOSE] = G2U("ѡ�񿨳�4ͼƬ");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP | ISFILE; i[NUMBER] = 4; i[BITMAPC] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("ͼƬ�ƶ����ٶ�"); i[CONFIGNAME] = "first screen Picture speed"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 10000; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("����һ��0-10000֮�������"); i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�ر�����"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();

	j["pages"].push_back(p); p.clear();
	p[TITLE] = G2U("�����뿨��");
	i[NAME] = G2U("����1����"); i[CONFIGNAME] = "namesfile1"; i[FILECHOOSE] = G2U("ѡ�񿨳�1����"); i[FILETYPE] = "nameFile"; i[LIMIT] = ISFILE; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����2����"); i[CONFIGNAME] = "namesfile2"; i[FILECHOOSE] = G2U("ѡ�񿨳�2����"); i[FILETYPE] = "nameFile"; i[LIMIT] = ISFILE; i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����3����"); i[CONFIGNAME] = "namesfile3"; i[FILECHOOSE] = G2U("ѡ�񿨳�3����"); i[FILETYPE] = "nameFile"; i[LIMIT] = ISFILE; i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����4����"); i[CONFIGNAME] = "namesfile4"; i[FILECHOOSE] = G2U("ѡ�񿨳�4����"); i[FILETYPE] = "nameFile"; i[LIMIT] = ISFILE; i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�鿨ʱ���ֵ�Rֵ(RGB)"); i[CONFIGNAME] = "text red"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 255; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("����һ��0-255֮�������"); i[NUMBER] = 5;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�鿨ʱ���ֵ�Gֵ(RGB)"); i[CONFIGNAME] = "text green"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 255; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("����һ��0-255֮�������"); i[NUMBER] = 6;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�鿨ʱ���ֵ�Bֵ(RGB)"); i[CONFIGNAME] = "text blue"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 255; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("����һ��0-255֮�������"); i[NUMBER] = 7;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�鱳����"); i[CONFIGNAME] = "special"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT; i[MAX] = 4; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("����һ��0-4֮������֣�0��ʾ���ã�"); i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�ر�����"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] = G2U("��Ƶ");
	i[NAME] = G2U("����3����Ƶ"); i[CONFIGNAME] = "signal 3star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("ѡ�񵥷�3����Ƶ"); i[FILETYPE] = "video"; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����4����Ƶ"); i[CONFIGNAME] = "signal 4star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("ѡ�񵥷�4����Ƶ"); i[FILETYPE] = "video"; i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����5����Ƶ"); i[CONFIGNAME] = "signal 5star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("ѡ�񵥷�5����Ƶ"); i[FILETYPE] = "video"; i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("ʮ��4����Ƶ"); i[CONFIGNAME] = "group 4star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("ѡ��ʮ��4����Ƶ"); i[FILETYPE] = "video"; i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("ʮ��5����Ƶ"); i[CONFIGNAME] = "group 5star video"; i[LIMIT] = ISFILE; i[FILECHOOSE] = G2U("ѡ��ʮ��5����Ƶ"); i[FILETYPE] = "video"; i[NUMBER] = 5;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�ر���Ƶ"); i[CONFIGNAME] = "off video"; i[ISSWITCH] = 1; i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�ر�����"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] = G2U("������(������Ч)");
	i[NAME] = G2U("������"); i[CONFIGNAME] = "open float window"; i[ISSWITCH] = 1; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("��ʼx����"); i[CONFIGNAME] = "float window x"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] = G2U("��С���ܴ�����Ļ"); i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("��ʼy����"); i[CONFIGNAME] = "float window y"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] = G2U("��С���ܴ�����Ļ"); i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("���"); i[CONFIGNAME] = "float window width"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] = G2U("��С���ܴ�����Ļ"); i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�߶�"); i[CONFIGNAME] = "float window height"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = MAXWINDOWSIZE; i[MIN] = 1;
	i[OUTOFLIMIT] = G2U("��С���ܴ�����Ļ"); i[NUMBER] = 5;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����ϵ��"); i[CONFIGNAME] = "float window Mu"; i[ISEDIT] = 1; i[LIMIT] = BETWEENCOUNT | REBOOT; i[MAX] = 1; i[MIN] = 0;
	i[OUTOFLIMIT] = G2U("��С���ܳ���1��С��0"); i[NUMBER] = 6;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("������ͼƬ"); i[CONFIGNAME] = "float window picture"; i[FILECHOOSE] = G2U("ѡ��������ͼƬ");
	i[FILETYPE] = "picture"; i[LIMIT] = ISBITMAP | REBOOT; i[NUMBER] = 11; i[BITMAPC] = FLOATPIC;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�ر�����"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 12;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	p[TITLE] = G2U("����");
	i[NAME] = G2U("����ģʽ"); i[CONFIGNAME] = "window mode(not full screen)"; i[ISSWITCH] = 1; i[NUMBER] = 1;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����"); i[CONFIGNAME] = "title name"; i[ISEDIT] = 1; i[LIMIT] = S_WINDOWTITLE; i[NUMBER] = 2;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����ҳ��json�ļ�����"); i[CONFIGNAME] = "cancel setting json file"; i[ISSWITCH] = 1; i[NUMBER] = 3;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�������ģʽ(������������ٿ�)"); i[CONFIGNAME] = "the new font is unsuit"; i[ISSWITCH] = 1; i[NUMBER] = 4;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("�ر�����"); i[CONFIGNAME] = "off music"; i[ISSWITCH] = 1; i[NUMBER] = 11;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("����ģʽ"); i[ISSWITCH] = 1; i[NUMBER] = 5; i[CONFIGNAME] = "debug mode"; i[LIMIT] = REBOOT;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("ʡ�ڴ�ģʽ"); i[ISSWITCH] = 1; i[NUMBER] = 6; i[CONFIGNAME] = "small memory"; i[LIMIT] = REBOOT;
	p["item"].push_back(i); i.clear();
	i[NAME] = G2U("ͼƬ��ԴĿ¼"); i[ISEDIT] = 1; i[NUMBER] = 7; i[CONFIGNAME] = "ImageDirectory"; i[LIMIT] = REBOOT; i[ISDIR] = 1;
	p["item"].push_back(i); i.clear();
	j["pages"].push_back(p); p.clear();
	// д��JSON�ļ�
	if (!config::getint(NOSETTINGFILE)) {
		std::ofstream file(jsonpath);
		if (file.is_open()) {
			try
			{
				slog << slog.pt() << "[INFO]begin to write json items";
				file << j.dump(4); // ��JSON����д���ļ���4��ʾ����4���ո�
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
// UTF8�ַ���ת��GBK�ַ���
std::string set2::U2G(const std::string& utf8)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];//��1���ڽض��ַ��� 
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
// GBK�ַ���ת��jsonʶ���UTF8�ַ���
std::string set2::G2U(const std::string& gbk)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//��1���ڽض��ַ��� 
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), gbk.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
