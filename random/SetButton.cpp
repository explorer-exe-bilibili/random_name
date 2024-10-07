#include "SetButton.h"

#include <shtypes.h>
#include <windowsx.h>
#include <shlobj.h>

#include "Button.h"
#include"configitem.h"
#include "VideoPlayer.h"
#include "explorer.h"
#include "getname.h"
#include "Gp.h"
#include "mywindows.h"
#include "set-json.h"
#include "sth2sth.h"
#include "ui.h"


using namespace std;

bool SetButton::needReran= false;
bool SetButton::needReboot = false;

void SetButton::OpenFile() const
{
	const wstring path = config::getpath(item.ConfigName);
	ShellExecute(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void SetButton::ChooseFile() const
{
	OPENFILENAMEW ofn = {};
	wchar_t strFilename[MAX_PATH] = {}; // 用于接收文件名

	ofn.lStructSize = sizeof(OPENFILENAMEW); // 结构体大小
	ofn.hwndOwner = mywindows::main_hwnd; // 拥有者窗口句柄,为NULL表示对话框是非模态的,实际应用中一般都要有这个句柄
	if (item.FileType == "All")
		ofn.lpstrFilter = L"所有文件\0 *.*\0\0";
	else if (item.FileType == "bmp")
		ofn.lpstrFilter = L"bmp图片文件(*.bmp)\0 * .bmp\0\0";
	else if (item.FileType == "nameFile")
		ofn.lpstrFilter = L"姓名文件(*.txt)\0 * .txt\0\0";
	else if (item.FileType == "video")
		ofn.lpstrFilter = L"视频文件(*.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v)\0 *.avi; *.mpg; *.mpeg; *.m2v; *.vob; *.mp4; *.m4v; *.mp4v; *.3gp; *.3gp2; *.wmv; *.asf; *.mov; *.qt; *.rm; *.rmvb; *.flv; *.f4v\0\0";
	else if (item.FileType == "picture")
		ofn.lpstrFilter = L"图片文件(*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf)\0*.jpg;*.jpeg;*.bmp;*.png;*.tif;*.tiff;*.gif;*.wmf;*.emf\0\0";
	ofn.nFilterIndex = 1; // 过滤器索引
	ofn.lpstrFile = strFilename; // 接收返回的文件名,注意第一个字符需要为NULL
	ofn.nMaxFile = sizeof(strFilename); // 缓冲区长度
	ofn.lpstrInitialDir = nullptr; // 初始目录为默认
	ofn.lpstrTitle = item.FileChooseWindowName.c_str(); // 使用系统默认标题留空即可
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; // 文件、目录必须存在,隐藏只读选项
	if (GetOpenFileNameW(&ofn))
	{
		if (item.FileType == "nameFile") {
			needReran = true;
		}
		const std::wstring filename(strFilename);
		config::replace(item.ConfigName, filename);
		Edit_SetText(textboxHwnd, filename.c_str());
		if (item.FileType == "picture")
		{
			explorer* ptr = explorer::getInstance();
			ptr->reloadBitmap(item.BitmapNumber);
		}
	}
}

void SetButton::ChooseDir() const
{
	BROWSEINFOW bi = {};
	bi.lpszTitle = item.FileChooseWindowName.c_str();
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	bi.hwndOwner = mywindows::main_hwnd;

	if (const LPITEMIDLIST piddle = SHBrowseForFolderW(&bi); piddle != nullptr)
	{
		if (wchar_t path[MAX_PATH]; SHGetPathFromIDListW(piddle, path))
		{
			const std::wstring dirPath(path);
			config::replace(item.ConfigName, dirPath);
			Edit_SetText(textboxHwnd, dirPath.c_str());
		}
		CoTaskMemFree(piddle);
	}
}

void SetButton::FullScreen()
{
	if (const long style=GetWindowStyle(mywindows::main_hwnd); style & WS_POPUP) {
		SetWindowLong(mywindows::main_hwnd, GWL_STYLE, style & (~WS_POPUP)|WS_SIZEBOX|WS_CAPTION);
		SetWindowPos(mywindows::main_hwnd, nullptr, 0, 0, mywindows::screenWidth * 0.5, mywindows::screenHeight * 0.5, SWP_FRAMECHANGED);
	}
	else
	{
		SetWindowLong(mywindows::main_hwnd, GWL_STYLE, ((style | WS_POPUP)&(~WS_SIZEBOX))&(~WS_CAPTION));
		SetWindowPos(mywindows::main_hwnd, nullptr, 0, 0, mywindows::screenWidth, mywindows::screenHeight, SWP_FRAMECHANGED);
	}
}

void SetButton::load()
{
	setPoint();
	if(item.Name.empty())return;
	if((item.IsFile||item.IsDir)&&ButtonRect.size()>=2)
	{
		Button b2;
		Button b1;
		b1.setBmapC(setbutton);
		b1.setxy2WWWH(ButtonRect[0].x, ButtonRect[0].y, ButtonRect[0].xend, ButtonRect[0].yend);
		b1.setText(L"选择");
		b1.setTextColor(0, 0, 0);
		b1.bind([this] ()
		{
			const wstring last_config = config::getpath(item.ConfigName);
			if (item.IsFile)
				ChooseFile();
			else
				ChooseDir();
			if (item.Limit & REBOOT)
			{
				if(last_config!=config::getpath(item.ConfigName))
				{
					needReboot = true;
				}
			}
		});
		b1.setMusic(CLICK_MUSIC);
		b1.setFont(&ui::text_mid);
		b1.refresh();
		b2.setxy2WWWH(ButtonRect[1].x, ButtonRect[1].y, ButtonRect[1].xend, ButtonRect[1].yend);
		b2.setText(L"打开");
		b2.bind([this]() {OpenFile(); });
		b2.setFont(&ui::text_mid);
		b2.setMusic(CLICK_MUSIC);
		b2.setTextColor(0, 0, 0);
		b2.setBmapC(setbutton);
		b2.refresh();
		buttons.push_back(b1);
		buttons.push_back(b2);
	}
	else if(item.IsSwitch&&!ButtonRect.empty())
	{
		Button b;
		b.setxy2WWWH(ButtonRect[0].x, ButtonRect[0].y, ButtonRect[0].xend, ButtonRect[0].yend);
		b.setBmapC(setbutton);
		b.bind([this]()
		{
			config::turnUpSideDown(item.ConfigName);
			if (item.ConfigName == INWINDOW)FullScreen();
			if(item.Limit&REBOOT)
			{
				needReboot = true;
			}
			buttons[0].setText(config::getint(item.ConfigName) == 1 ? L"开" : L"关");
			set2::rereadConfig();
			InvalidateRect(mywindows::main_hwnd, nullptr, false);			
		});
		b.setText(config::getint(item.ConfigName) == 1 ? L"开" : L"关");
		b.setTextColor(0, 0, 0);
		b.setMusic(CLICK_MUSIC);
		buttons.push_back(b);
	}
	refresh();
}

HWND SetButton::CreateEditBox(const HWND hWndParent, const int number, const point& rect, const wchar_t* words) {
	// 创建EDIT控件的样式
	constexpr DWORD editStyle = ES_AUTOHSCROLL | (WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER);

	// 创建EDIT控件的附加样式（可选）
	constexpr DWORD editExStyle = WS_EX_CLIENTEDGE;
	const int x = rect.x * mywindows::WW;
	const int y = rect.y * mywindows::WH;
	const int w = rect.xend * mywindows::WW - x;
	const int h = rect.yend * mywindows::WH - y;
	// 创建文本框
	const HWND hEdit = CreateWindowExW(editExStyle, L"EDIT", words, editStyle,
		x, y, w, h, hWndParent, HMENU(number), nullptr, nullptr);

	// 返回文本框句柄
	return hEdit;
}


void SetButton::EditBoxEditor(const std::wstring& tmp) const
{
	if (item.Limit & S_WINDOWTITLE)
	{
		SetWindowTextW(mywindows::main_hwnd, tmp.c_str());
	}
	if (item.Limit & BETWEENCOUNT)
	{
		int value;
		try
		{
			value = std::stoi(tmp);
		}
		catch (const std::exception& e)
		{
			mywindows::errlogf << "[ERROR]meet a error :" << e.what() << '\n';
			MessageBox(nullptr, L"请输入数字", L"错误", MB_ICONERROR);
			return;
		}
		if (value<item.min || value>item.max)
		{
			MessageBoxA(nullptr, sth2sth::UTF_82ASCII(item.OutOfLimitOutPut).c_str(), R"(错误)", MB_ICONERROR);
			return;
		}
	}
	if (item.Limit & REBOOT)
	{
		if (tmp == config::get(item.ConfigName))return;
		if (tmp == config::getpath(item.ConfigName))return;
		needReboot = true;
	}
	if (item.IsFile)
	{
		if (!std::filesystem::exists(tmp))return;
		if (tmp == config::getpath(item.ConfigName))return;
		if (item.FileType == "nameFile")
		{
			for (char i = 0; i <= config::getint(POOL_COUNT); i++)
				getname::getInstance()->ReRandom(i);
		}
	}
	config::replace(item.ConfigName, tmp);
	if (item.Limit & ISBITMAP)
	{
		explorer::getInstance()->reloadBitmap(item.BitmapNumber);
	}
	if (item.FileType == "video")
	{
		explorer::getInstance()->reloadVideo(item.ConfigName);
	}
}

SetButton::SetButton(const sitem& Item)
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
	TextBoxRect = {};
	item = {};
	buttons.clear();
	p = nullptr;
}

void SetButton::setItem(const sitem& Item)
{
	item = Item;
	load();
}

void SetButton::show()
{
	if (!item.Name.empty())
	{
		const int x = TitleRect.x * mywindows::WW;
		const int y = TitleRect.y * mywindows::WH;
		p->DrawString(item.Name, ui::text_mid, x, y, 236, 229, 216);
	}
	for (auto& i : buttons)
	{
		i.paint();
	}
	if (item.IsEditBox)
	{
		if (textboxHwnd == nullptr) {
			textboxHwnd = CreateEditBox(mywindows::main_hwnd, item.Number, TextBoxRect,
				config::get(item.ConfigName).c_str());
			if (item.IsFile)
			{
				EditBoxEditor(config::getpath(item.ConfigName));
			}
			else
				EditBoxEditor(config::get(item.ConfigName));
		}
	}
}

void SetButton::unshow()
{
	if (item.IsEditBox&&textboxHwnd!= nullptr)
	{
		DestroyWindow(textboxHwnd);
		textboxHwnd= nullptr;
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
	const int i = item.Number;
	const double x = (i <= 10) ? 0.05 : ((i > 10 && i <= 19) ? (0.55) : 0);
	const double y = (i <= 10) ? (i * 0.09) : ((i > 10 && i <= 19) ? ((i - 9) * 0.09) : 0);
	TitleRect = { x,y,0,0 };
	if (item.IsEditBox) {
		TextBoxRect = { x + 0.2,y,x + 0.3,y + 0.07 };
		if (item.IsFile||item.IsDir) {
			point temp = {x + 0.3, y, x + 0.35, y + 0.07};
			ButtonRect.push_back(temp);
			temp = { x + 0.35,y,x + 0.4,y + 0.07 };
			ButtonRect.push_back(temp);
		}
	}
	else if (item.IsSwitch) {
		const point temp = {x + 0.2, y, x + 0.3, y + 0.07};
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
	const int x = TextBoxRect.x * mywindows::WW;
	const int y = TextBoxRect.y * mywindows::WH;
	const int w = TextBoxRect.xend * mywindows::WW - x;
	const int h = TextBoxRect.yend * mywindows::WH - y;
	if (textboxHwnd!= nullptr)
		MoveWindow(textboxHwnd, x, y, w, h, 1);
}

void SetButton::reConnect()
{
	for(auto&i:buttons)
	{
		i.reConnect();
	}
}

void SetButton::click(const int x, const int y) const
{
	for(auto&i:buttons)
	{
		i.click(CLICK, x, y);
	}
}

void SetButton::EditBoxUpgrade(const int number) const
{
	if (item.Number == number)
	{
		wchar_t tmp[MAX_PATH];
		GetWindowTextW(textboxHwnd, tmp, MAX_PATH);
		const std::wstring temps(tmp);
		if (temps.empty())return;
		EditBoxEditor(temps);
	}
}
