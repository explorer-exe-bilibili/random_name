#include "SetButton.h"

#include <windowsx.h>

#include "config.h"
#include "directshow.h"
#include "mywindows.h"
#include "sth2sth.h"
#include "ui.h"


using namespace std;

bool SetButton::reran= 0;

void SetButton::OpenFile(sitem item)
{
	wstring path = config::getpath(item.ConfigName);
	ShellExecute(NULL, L"open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void SetButton::ChooseFile(sitem item)
{
	OPENFILENAMEW ofn = { 0 };
	wchar_t strFilename[MAX_PATH] = { 0 }; // ���ڽ����ļ���

	ofn.lStructSize = sizeof(OPENFILENAMEW); // �ṹ���С
	ofn.hwndOwner = mywindows::main_hwnd; // ӵ���ߴ��ھ��,ΪNULL��ʾ�Ի����Ƿ�ģ̬��,ʵ��Ӧ����һ�㶼Ҫ��������
	if (item.FileType == "All")
		ofn.lpstrFilter = L"�����ļ�\0 *.*\0\0";
	else if (item.FileType == "bmp")
		ofn.lpstrFilter = L"bmpͼƬ�ļ�(*.bmp)\0 * .bmp\0\0";
	else if (item.FileType == "nameFile")
		ofn.lpstrFilter = L"�����ļ�(*.txt)\0 * .txt\0\0";
	else if (item.FileType == "video")
		ofn.lpstrFilter = L"��Ƶ�ļ�(*.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v)\0 *.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v\0\0";
	else if (item.FileType == "picture")
		ofn.lpstrFilter = L"ͼƬ�ļ�(*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf)\0*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf\0\0";
	ofn.nFilterIndex = 1; // ����������
	ofn.lpstrFile = strFilename; // ���շ��ص��ļ���,ע���һ���ַ���ҪΪNULL
	ofn.nMaxFile = sizeof(strFilename); // ����������
	ofn.lpstrInitialDir = NULL; // ��ʼĿ¼ΪĬ��
	ofn.lpstrTitle = item.FileChooseWindowName.c_str(); // ʹ��ϵͳĬ�ϱ������ռ���
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; // �ļ���Ŀ¼�������,����ֻ��ѡ��
	if (GetOpenFileNameW(&ofn))
	{
		if (item.FileType == "nameFile") {
			reran = 1;
		}
		std::wstring filename(strFilename);
		config::replace(item.ConfigName, filename);
		Edit_SetText(textboxhwnd, filename.c_str());
	}
}

void SetButton::load()
{
	setPoint();
	if(item.Name==L"")return;
	Button b1, b2;
	Button b;
	if(item.IsFile&&ButtonRect.size()>=2)
	{
		b1.setBmapC(setbutton);
		b1.setxy2WWWH(ButtonRect[0].x, ButtonRect[0].y, ButtonRect[0].xend, ButtonRect[0].yend);
		b1.setText(L"ѡ��");
		b1.setTextColor(0, 0, 0);
		b1.bind([this] (){ChooseFile(item); });
		b1.setMusic(CLICK_MUSIC);
		b1.setFont(&ui::text_mid);
		b1.refresh();
		b2.setxy2WWWH(ButtonRect[1].x, ButtonRect[1].y, ButtonRect[1].xend, ButtonRect[1].yend);
		b2.setText(L"��");
		b2.bind([this]() {OpenFile(item); });
		b2.setFont(&ui::text_mid);
		b2.setMusic(CLICK_MUSIC);
		b2.setTextColor(0, 0, 0);
		b2.setBmapC(setbutton);
		b2.refresh();
		buttons.push_back(b1);
		buttons.push_back(b2);
	}
	else if(item.IsSwitch&&!(ButtonRect.empty()))
	{
		b.setxy2WWWH(ButtonRect[0].x, ButtonRect[0].y, ButtonRect[0].xend, ButtonRect[0].yend);
		b.setBmapC(setbutton);
		b.bind([this]() {config::turnUpSideDown(item.ConfigName); });
		b.setText(config::getint(item.ConfigName) == 1 ? L"��" : L"��");
		b.setTextColor(0, 0, 0);
		buttons.push_back(b);
	}
	refresh();
}

HWND SetButton::CreateEditBox(HWND hWndParent, int number, point rect, const wchar_t* words) {
	// ����EDIT�ؼ�����ʽ
	DWORD editStyle = ES_AUTOHSCROLL | (WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER);

	// ����EDIT�ؼ��ĸ�����ʽ����ѡ��
	DWORD editExStyle = WS_EX_CLIENTEDGE;
	int x, y, w, h;
	x = rect.x * mywindows::WW;
	y = rect.y * mywindows::WH;
	w = rect.xend * mywindows::WW - x;
	h = rect.yend * mywindows::WH - y;
	// �����ı���
	HWND hEdit = CreateWindowExW(editExStyle, L"EDIT", words, editStyle, x, y, w, h, hWndParent, (HMENU)number, NULL, NULL);

	// �����ı�����
	return hEdit;
}


void SetButton::EditBoxEditor(sitem item, std::wstring tmp)
{
	if (item.Limit & S_WINDOWTITLE)
	{
		SetWindowTextW(mywindows::main_hwnd, tmp.c_str());
	}
	else if (item.Limit & BETWEENCOUNT)
	{
		int value;
		try
		{
			value = std::stoi(tmp);
		}
		catch (const std::exception& e)
		{
			mywindows::errlogf << "[ERROR]meet a error :" << e.what() << std::endl;
			MessageBox(NULL, L"����������", L"����", MB_ICONERROR);
			return;
		}
		if (value<item.min || value>item.max)
		{
			MessageBoxA(NULL, sth2sth::UTF_82ASCII(item.OutOfLimitOutPut).c_str(), "����", MB_ICONERROR);
			return;
		}
	}
	else if (item.IsFile)
	{
		if (!std::filesystem::exists(tmp))return;
		if (tmp == Log::wrunpath + config::get(item.ConfigName))return;
	}
	config::replace(item.ConfigName, tmp);
}

SetButton::SetButton(sitem Item)
{
	setItem(Item);
}

SetButton::SetButton()
{
	item = {};
}

SetButton::~SetButton()
{
	TitleRect = {};
	ButtonRect = {};
	TextBoxRext = {};
	item = {};
	buttons.clear();
	p = nullptr;
}

void SetButton::setItem(sitem Item)
{
	item = Item;
	load();
}

void SetButton::show()
{
	if (!item.Name.empty())
	{
		int x = TitleRect.x * mywindows::WW;
		int y = TitleRect.y * mywindows::WH;
		p->DrawString(item.Name, ui::text_mid, x, y, 236, 229, 216);
	}
	for (auto& i : buttons)
	{
		i.paint();
	}
	if (item.IsEditBox)
	{
		if (textboxhwnd == NULL) {
			textboxhwnd = CreateEditBox(mywindows::main_hwnd, item.Number, TextBoxRext,
				config::get(item.ConfigName).c_str());
			if (item.IsFile)
			{
				EditBoxEditor(item, config::getpath(item.ConfigName));
			}
			else
				EditBoxEditor(item, config::get(item.ConfigName));
		}
	}
}

void SetButton::unshow()
{
	if (item.IsEditBox&&textboxhwnd!=NULL)
	{
		DestroyWindow(textboxhwnd);
		textboxhwnd=NULL;
	}
}

void SetButton::setGp(Gp* p)
{
	this->p = p;
	for(auto& i:buttons)
	{
		i.setGp(p);
	}
}


void SetButton::setPoint() {
	int i = item.Number;
	double x = (i <= 10) ? 0.05 : ((i > 10 && i <= 19) ? (0.55) : 0);
	double y = (i <= 10) ? (i * 0.09) : ((i > 10 && i <= 19) ? ((i - 9) * 0.09) : 0);
	TitleRect = { x,y,0,0 };
	if (item.IsEditBox) {
		TextBoxRext = { x + 0.2,y,x + 0.3,y + 0.07 };
		if (item.IsFile) {
			point temp = {x + 0.3, y, x + 0.35, y + 0.07};
			ButtonRect.push_back(temp);
			temp = { x + 0.35,y,x + 0.4,y + 0.07 };
			ButtonRect.push_back(temp);
		}
	}
	else if (item.IsSwitch) {
		point temp = {x + 0.2, y, x + 0.3, y + 0.07};
		ButtonRect.push_back(temp);
	}
	refresh();
}

void SetButton::refresh()
{
	for (auto& i : buttons)
	{
		i.refresh();
	}
	int x, y, w, h;
	x=TextBoxRext.x * mywindows::WW;
	y=TextBoxRext.y * mywindows::WH;
	w=TextBoxRext.xend * mywindows::WW - x;
	h=TextBoxRext.yend * mywindows::WH - y;
	MoveWindow(textboxhwnd, x, y, w, h, 1);
}

void SetButton::reConnect()
{
	for(auto&i:buttons)
	{
		i.reConnect();
	}
}

void SetButton::click(int x,int y)
{
	for(auto&i:buttons)
	{
		i.click(CLICK, x, y);
	}
}
