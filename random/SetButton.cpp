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

void SetButton::ChooseColor_()
{
	CHOOSECOLORW cc = {};
	COLORREF crCustColors[16] = {};
	cc.lStructSize = sizeof(CHOOSECOLORW);
	cc.hwndOwner = mywindows::main_hwnd;
	cc.lpCustColors = crCustColors;
	cc.rgbResult = config::getd(item.ConfigName);
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	if (ChooseColorW(&cc))
	{
		const COLORREF color = 255<<24|(cc.rgbResult);
		config::replace(item.ConfigName, std::to_wstring(color));
		std::wstringstream wss;
		wss << std::hex << std::uppercase << color;
		const std::wstring wstr= L"0x"+wss.str();
		buttons[0].setText(wstr);
		buttons[0].setBkColor(color);
	}
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
		const std::wstring filename(strFilename);
		config::replace(item.ConfigName, filename);
		//editBox->SetText(filename);
		SetWindowText(textBoxhwnd, filename.c_str());
		if (item.FileType == "nameFile") {
			needReran = true;
		}
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
			//editBox->SetText(dirPath);
			SetWindowText(textBoxhwnd, dirPath.c_str());
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
		b1.setTextColor(ARGB(255,0,0,0));
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
		b2.setTextColor(ARGB(255 ,0, 0, 0));
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
		b.setTextColor(ARGB(255,0,0,0));
		b.setMusic(CLICK_MUSIC);
		buttons.push_back(b);
	}
	else if(item.IsColor&&ButtonRect.size()>=2)
	{
		Button b,b1;
		b.setxy2WWWH(ButtonRect[0].x, ButtonRect[0].y, ButtonRect[0].xend, ButtonRect[0].yend);
		b.setDisableBmap(true);
		b.setDisableBkColor(false);
		b.setBkColor(config::getd(item.ConfigName));
		std::wstringstream wss;
		wss << std::hex << std::uppercase << uint32_t(config::getd(item.ConfigName));
		const std::wstring wstr = L"0x" + wss.str();
		b.setText(wstr);
		b.setTextColor(ARGB(255, 0, 0, 0));
		b.setFont(&ui::text);
		buttons.push_back(b);
		b1.setxy2WWWH(ButtonRect[1].x, ButtonRect[1].y, ButtonRect[1].xend, ButtonRect[1].yend);
		b1.setBmapC(setbutton);
		b1.bind([this]()
		{
			ChooseColor_();
			InvalidateRect(mywindows::main_hwnd, nullptr, false);	
		});
		b1.setText(L"选择颜色");
		b1.setTextColor(ARGB(255,0,0,0));
		b1.setMusic(CLICK_MUSIC);
		buttons.push_back(b1);
	}
	refresh();
}

//MyEditBox* SetButton::CreateEditBox(const HWND hWndParent, const int number, const point& rect, const wchar_t* words) {
//	// 创建EDIT控件的样式
//	constexpr DWORD editStyle = ES_AUTOHSCROLL | (WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER);
//
//	// 创建EDIT控件的附加样式（可选）
//	constexpr DWORD editExStyle = WS_EX_CLIENTEDGE;
//	const int x = rect.x * mywindows::WW;
//	const int y = rect.y * mywindows::WH;
//	const int w = rect.xend * mywindows::WW - x;
//	const int h = rect.yend * mywindows::WH - y;
//	// 创建文本框
//	MyEditBox* pEdit = new MyEditBox(hWndParent, { x,y,x + w,y + h },number);
//	pEdit->SetText(words);
//	pEdit->Show();
//	// 返回文本框句柄
//	return pEdit;
//}

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
	HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", words, editStyle
		, x, y, w, h, hWndParent, HMENU(number), nullptr, nullptr);
	// 返回文本框句柄
	return hwnd;
}

void SetButton::EditBoxEditor(const std::wstring& tmp) const
{
	if (item.IsColor)return;
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
		p->DrawString(item.Name, ui::text_mid, x, y, ARGB(255, 236, 229, 216));
	}
	for (auto& i : buttons)
	{
		i.paint();
	}
	if (item.IsEditBox)
	{
		if(textBoxhwnd==nullptr)
		{
			wstring tmp = config::get(item.ConfigName);
			if (item.IsDir || item.IsFile)tmp = config::getpath(item.ConfigName);
			textBoxhwnd = CreateEditBox(mywindows::main_hwnd, item.Number, TextBoxRect,tmp.c_str());
		}
	}
}

void SetButton::unshow()
{
	if (item.IsEditBox && textBoxhwnd != nullptr)
	{
		DestroyWindow(textBoxhwnd);
		textBoxhwnd = nullptr;
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
	else if (item.IsColor)
	{
		point temp = { x + 0.2,y,x + 0.3,y + 0.07 };
		ButtonRect.push_back(temp);
		temp = { x + 0.3,y,x + 0.4,y + 0.07 };
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
	/*	if(editBox)
		GetWindowTextW(editBox->GetHwnd(), tmp, MAX_PATH);*/
		GetWindowText(textBoxhwnd, tmp, MAX_PATH);
		const std::wstring temps(tmp);
		if (temps.empty())return;
		EditBoxEditor(temps);
	}
}

//WNDPROC MyEditBox::oldProc = nullptr;
//
//MyEditBox::MyEditBox(HWND parent, const RECT& rect, int id)
//	: hwnd(nullptr), parent_hwnd(parent), bgColor(RGB(255, 255, 255)), hBrush(nullptr), rect(rect), id(id)
//{
//	// 创建EDIT控件的样式
//	constexpr DWORD editStyle = ES_AUTOHSCROLL | (WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT | WS_BORDER);
//
//	// 创建EDIT控件的附加样式（可选）
//	constexpr DWORD editExStyle = WS_EX_CLIENTEDGE;
//	if (hwnd != nullptr)return;
//	int x = rect.left;
//	int y = rect.top;
//	int w = rect.right - rect.left;
//	int h = rect.bottom - rect.top;
//	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", editStyle
//		, x, y, w, h, mywindows::main_hwnd, HMENU(id), nullptr, nullptr);
//	SetWindowLongPtr(hwnd, GWLP_USERDATA, LONG_PTR(this));
//	oldProc = WNDPROC(SetWindowLongPtr(hwnd, GWLP_WNDPROC, LONG_PTR(EditBoxProc)));
//	SetBackgroundColor(bgColor);
//	SetFocus(hwnd);
//}
//
//void MyEditBox::SetText(const std::wstring& text) const
//{
//	SetWindowText(hwnd, text.c_str());
//}
//
//std::wstring MyEditBox::GetText() const
//{
//	int length = GetWindowTextLength(hwnd);
//	std::wstring text(length, L'\0');
//	GetWindowText(hwnd, &text[0], length + 1);
//	return text;
//}
//
//void MyEditBox::SetBackgroundColor(COLORREF color)
//{
//	bgColor = color;
//	if (hBrush)
//	{
//		DeleteObject(hBrush);
//	}
//	hBrush = CreateSolidBrush(bgColor);
//	InvalidateRect(hwnd, NULL, TRUE);
//}
//
//void MyEditBox::Show() const
//{
//	ShowWindow(hwnd, SW_SHOW);
//}
//
//void MyEditBox::Unshow() const
//{
//	ShowWindow(hwnd, SW_HIDE);
//}
//
//void MyEditBox::MoveTo(RECT new_rect)
//{
//	rect = new_rect;
//	MoveWindow(hwnd, new_rect.left, new_rect.top, new_rect.right - new_rect.left, new_rect.bottom - new_rect.top, TRUE);
//}
//
//LRESULT CALLBACK MyEditBox::EditBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	MyEditBox* pThis = (MyEditBox*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
//	if (pThis)
//	{
//		switch (msg)
//		{
//		case WM_CTLCOLOREDIT:
//		{
//			HDC hdc = (HDC)wParam;
//			SetBkColor(hdc, pThis->bgColor);
//			return (LRESULT)pThis->hBrush;
//		}
//		default:
//			break;
//		}
//	}
//	return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
//}
