#include"genthen-impact.h"

LRESULT CALLBACK WinSunProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hdc = GetDC(NULL);
	mywindows::hWnd = hwnd;
	if (setting::reran)paintname::rerandom();
	switch (uMsg) {//通过判断消息进行消息响应
	case WM_CREATE: {
		// 加载两个背景图片
		hbitmaps[background] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\wish-background.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[over1] = (HBITMAP)LoadImage(NULL, config::get(OVER1).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[over2] = (HBITMAP)LoadImage(NULL, config::get(OVER2).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[over3] = (HBITMAP)LoadImage(NULL, config::get(OVER3).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[over4] = (HBITMAP)LoadImage(NULL, config::get(OVER4).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[cardbackground] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\cardbg.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[pink1b] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\1pinkb.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[pink1i] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\1pinki.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[pink10i] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\10pinki.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[pink10b] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\10pinkb.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[blue1b] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\1blueb.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[blue1i] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\1bluei.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[exiti] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\exiti.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[exitb] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\exitb.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[buttom] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\buttom.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[blue10i] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\10bluei.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[blue10b] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\10blueb.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[goldencardbg] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\goldenbg.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[listbg] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\list.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[list3] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\3list.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[list4] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\4list.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[list5] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\5list.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[list6] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\fulllist.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[setbm] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\setting.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hbitmaps[setbutton] = (HBITMAP)LoadImage(NULL, L".\\files\\imgs\\settingbuttom.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		int desiredPixelHeight = mywindows::windowWidth * 0.17;
		// 获取设备上下文的 DPI
		HDC hdc = GetDC(NULL); // 获取桌面设备上下文
		int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
		// 计算逻辑单位高度
		int logicalHeight = MulDiv(desiredPixelHeight, 72, dpi);
		int logicalweidth = logicalHeight * 0.77;
		paintname::icon_star = CreateFontW(logicalHeight * 0.0862, logicalweidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon_mid = CreateFontW(logicalHeight * 0.16, logicalweidth * 0.22, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::icon = CreateFontW(logicalHeight * 0.2299, logicalweidth * 0.3008, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
		ui::text = CreateFontW(logicalHeight * 0.1149, logicalweidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		ui::text_mid = CreateFontW(logicalHeight * 0.1724, logicalweidth * 0.1729, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		ui::text_big = CreateFontW(logicalHeight, logicalweidth, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		ui::text_list = CreateFontW(logicalHeight * 0.7, logicalweidth * 0.7, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		ReleaseDC(NULL, hdc);
		if (ui::text == NULL)mywindows::errlog("text font load error");
		if (ui::icon == NULL)mywindows::errlog("icon font load error");
		if (ui::text_big == NULL)mywindows::errlog("icon font load error");
		// 获取图标位图信息
		GetObject(hbitmaps[background], sizeof(BITMAP), &bm);
		GetObject(hbitmaps[setbutton], sizeof(BITMAP), &setting::setbu);
		GetObject(hbitmaps[cardbackground], sizeof(BITMAP), &cardbg_);
		GetObject(hbitmaps[listbg], sizeof(BITMAP), &listbm_);
		GetObject(hbitmaps[exitb], sizeof(BITMAP), &setting::exitbm);
		GetObject(hbitmaps[goldencardbg], sizeof(BITMAP), &goldenbg);
		GetObject(hbitmaps[pink1b], sizeof(BITMAP), &ball);
		GetObject(hbitmaps[list3], sizeof(BITMAP), &list3star_);
		GetObject(hbitmaps[list4], sizeof(BITMAP), &list4star_);
		GetObject(hbitmaps[list5], sizeof(BITMAP), &list5star_);
		GetObject(hbitmaps[list6], sizeof(BITMAP), &list6star_);
		GetObject(hbitmaps[over1], sizeof(BITMAP), &overlay1Bm);
		GetObject(hbitmaps[over2], sizeof(BITMAP), &overlay2Bm);
		GetObject(hbitmaps[over3], sizeof(BITMAP), &overlay3Bm);
		GetObject(hbitmaps[over4], sizeof(BITMAP), &overlay4Bm);
		GetObject(hbitmaps[buttom], sizeof(BITMAP), &buttom_);
		GetObject(hbitmaps[setbm], sizeof(BITMAP), &setting::setbm_);
		// 打开并播放背景音乐
		mciSendString(L"open .\\files\\mp3\\backsound.mp3 alias bgm", NULL, 0, NULL); // 打开 MP3 文件并创建一个别名 'bgm'
		if (!setting::offmusic)
			mciSendString(L"play bgm repeat", NULL, 0, NULL); // 使用别名 'bgm' 播放音乐，并设置为循环播放
		SetWindowPos(mywindows::hWnd, HWND_TOP, 0, 0, mywindows::windowWidth, mywindows::windowHeight, SWP_NOZORDER | SWP_FRAMECHANGED);
		ReleaseDC(0, hdc);
		break;
	}
	case WM_PAINT: {
		RECT rect;
		GetClientRect(hwnd, &rect);
		// 计算窗口的宽度和高度
		mywindows::windowTop = rect.left;
		mywindows::windowLeft = rect.top;
		int tempint = rect.right - rect.left;
		if (tempint != mywindows::windowWidth)setting::initing = 1;
		mywindows::windowWidth = tempint;
		tempint = rect.bottom - rect.top;
		if (tempint != mywindows::windowHeight)setting::initing = 1;
		mywindows::windowHeight = tempint;
		if (setting::initing) {
			ui::ballW = 0.18 * mywindows::windowWidth;
			ui::ballH = 0.075 * mywindows::windowHeight;
			ui::ball10x = mywindows::windowWidth - ui::ballW * 1.3;
			ui::bally = mywindows::windowHeight - ui::ballH * 2;
			ui::ball1x = mywindows::windowWidth - ui::ballW * 2.4;
			ui::ball1end = ui::ball1x + ui::ballW;
			ui::ball10end = ui::ball10x + ui::ballW;
			ui::ballyend = ui::bally + ui::ballH;
			ui::overlay1X = mywindows::windowWidth * 3 / 9 - mywindows::windowWidth / 17;
			ui::overlay2X = mywindows::windowWidth * 4 / 9 - mywindows::windowWidth / 17;
			ui::overlay3X = mywindows::windowWidth * 5 / 9 - mywindows::windowWidth / 17;
			ui::overlay4X = mywindows::windowWidth * 6 / 9 - mywindows::windowWidth / 17;
			ui::overlay1Y = mywindows::windowHeight * 3 / 40;
			ui::overlayW = mywindows::windowWidth * 3 / 40;
			ui::overlayH = mywindows::windowHeight * 3 / 40;
			ui::button1x = ui::overlay1X + ui::overlayW;
			ui::button2x = ui::overlay2X + ui::overlayW;
			ui::button3x = ui::overlay3X + ui::overlayW;
			ui::button4x = ui::overlay4X + ui::overlayW;
			ui::buttony = ui::overlay1Y + ui::overlayH;
			ui::overX = mywindows::windowWidth / 10 * 2;
			ui::overlayY = mywindows::windowHeight / 10 * 2;
			ui::bottom1x = mywindows::windowWidth * 0.1;
			ui::bottom1y = mywindows::windowHeight * 0.85;
			ui::listyend = mywindows::windowHeight;
			paintname::skipbmx = mywindows::windowWidth * 0.8;
			paintname::skipbmy = mywindows::windowHeight * 0.045;
			paintname::skipbmxend = paintname::skipbmx + mywindows::windowWidth * 0.1;
			paintname::skipbmyend = paintname::skipbmy + 100;
			ui::settingx = mywindows::windowWidth * 0.05;
			ui::settingy = mywindows::windowHeight * 0.85;
			ui::settingxend = ui::settingx + mywindows::windowWidth * 0.023;
			ui::settingyend = ui::settingy + mywindows::windowHeight * 0.036;
			setting::exitx = mywindows::windowWidth * 0.9;
			setting::exitxend = mywindows::windowWidth * 0.93;
			setting::exity = mywindows::windowHeight * 0.045;
			setting::exityend = mywindows::windowHeight * 0.045 + mywindows::windowWidth * 0.03;
			ui::listx[0] = mywindows::windowWidth * 0.107;
			ui::listxend = mywindows::windowWidth * 0.078;
			ui::addnamex = mywindows::windowWidth * 0.4;
			ui::addnameendx = ui::addnamex + mywindows::windowWidth * 0.12;
			ui::addnamey = mywindows::windowHeight * 0.8;
			ui::addnameendy = ui::addnamey + mywindows::windowHeight * 0.036;
			setting::init();
			for (char i = 0; i <= 9; i++) {
				ui::listx[i + 1] = ui::listx[i] + ui::listxend;
			}
			if (setting::changebitmap[0]) {
				hbitmaps[over1] = (HBITMAP)LoadImage(NULL, config::get(OVER1).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				GetObject(hbitmaps[over1], sizeof(BITMAP), &overlay1Bm);
				setting::changebitmap[0] = 0;
			}
			if (setting::changebitmap[1]) {
				hbitmaps[over2] = (HBITMAP)LoadImage(NULL, config::get(OVER2).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				GetObject(hbitmaps[over2], sizeof(BITMAP), &overlay2Bm);
				setting::changebitmap[1] = 0;
			}
			if (setting::changebitmap[2]) {
				hbitmaps[over3] = (HBITMAP)LoadImage(NULL, config::get(OVER3).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				GetObject(hbitmaps[over3], sizeof(BITMAP), &overlay3Bm);
				setting::changebitmap[2] = 0;
			}
			if (setting::changebitmap[3]) {
				hbitmaps[over4] = (HBITMAP)LoadImage(NULL, config::get(OVER4).c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				GetObject(hbitmaps[over4], sizeof(BITMAP), &overlay4Bm);
				setting::changebitmap[3] = 0;
			}
			DeleteObject(paintname::icon_star);
			DeleteObject(ui::text_mid);
			DeleteObject(ui::text_big);
			DeleteObject(ui::text);
			DeleteObject(ui::text_list);
			DeleteObject(ui::icon_mid);
			DeleteObject(ui::icon);
			int desiredPixelHeight = mywindows::windowWidth * 0.17;
			// 获取设备上下文的 DPI
			HDC hdc = GetDC(NULL); // 获取桌面设备上下文
			int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
			// 计算逻辑单位高度
			int logicalHeight = MulDiv(desiredPixelHeight, 72, dpi);
			int logicalweidth = logicalHeight * 0.77;
			paintname::icon_star = CreateFontW(logicalHeight * 0.0862, logicalweidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
			ui::icon_mid = CreateFontW(logicalHeight * 0.16, logicalweidth * 0.22, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
			ui::icon = CreateFontW(logicalHeight * 0.2299, logicalweidth * 0.3008, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"genshin-icon");
			ui::text = CreateFontW(logicalHeight * 0.1149, logicalweidth * 0.1127, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
			ui::text_mid = CreateFontW(logicalHeight * 0.1724, logicalweidth * 0.1729, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
			ui::text_big = CreateFontW(logicalHeight, logicalweidth, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
			ui::text_list = CreateFontW(logicalHeight * 0.7, logicalweidth * 0.7, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"SDK_SC_Web");
		}
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(mywindows::hWnd, &ps);
		HDC hdcMem = CreateCompatibleDC(hdc);
		SetStretchBltMode(hdc, HALFTONE);
		switch (ui::screenmode)
		{
		case FIRST_MENU: {
			paintfirstscreen::printfirstmenu(hdc, hdcMem);
			//if (setting::initing)paintfirstscreen::printfirstmenu(hdc, hdcMem);
			//else if (!(setting::initing AND paintfirstscreen::firsttime)) {
			//    paintfirstscreen::printfirstmenu(hdc, hdcMem);
			//    mywindows::log("set mode %d", ui::mode);
			//    if (setting::offvideo)
			//        ui::creatbuttom(hdc, hdcMem, ui::bottom1x, ui::bottom1y, L"跳过视频:开");
			//    if (!setting::offvideo)
			//        ui::creatbuttom(hdc, hdcMem, ui::bottom1x, ui::bottom1y, L"跳过视频:关");
			//}
			//else if (paintfirstscreen::firsttime)
			//    if (ui::clicked) paintfirstscreen::printfirstmenu(hdc, hdcMem);
		}break;
		case SHOW_NAMES_ING: {
			setting::initing = 0;
			if (ui::printing) {
				if (ui::isball1) {
					paintname::out1name(hdc, hdcMem);
					ui::ing = 0;
					setting::initing = 1;
				}
				if (ui::isball10) {
					if (ui::ft) {
						if (!paintname::skipped)paintname::out10name(hdc, hdcMem);
						ui::ft = 0;
					}
					else if (ui::clicked)
						paintname::out10name(hdc, hdcMem);
					ui::ing = 0;
				}
			}
		}break;
		case SETTING: {
			setting::initing = 0;
			setting::paintsettingpage(hdc, hdcMem);
		}break;
		default:
			break;
		}
		mywindows::log("paint successfully");
		DeleteDC(hdcMem);
		EndPaint(mywindows::hWnd, &ps);
	}break;
	case WM_LBUTTONDOWN: {
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if (!ui::ing) {
			switch (ui::screenmode)
			{
			case FIRST_MENU:
				click::firstscreen(x, y);
				break;
			case SHOW_NAMES_ING:
				click::showname(x, y);
				break;
			case SETTING:
				click::setting(x, y);
				break;
			}
		}
		paintname::KillAllTimer();
		InvalidateRect(mywindows::hWnd, NULL, FALSE);
	}break;
	case WM_KEYDOWN: switch (wParam) {
	case VK_ESCAPE: {
		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}break;
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case EN_CHANGE: {
			switch (ui::screenmode)
			{
			case SETTING:setting::seteditbox(lParam, wParam); break;
			default:
				break;
			}
		}break;
		default:
			break;
		}
		break;
	case WM_DESTROY: {
		PostQuitMessage(0);
	}break;
	case WM_ACTIVATEAPP:
		switch (ui::screenmode) {
		case FIRST_MENU:
			paintfirstscreen::repaint();
			break;
		case SETTING:
			setting::repaint();
			break;
		}
	default:
		return DefWindowProc(mywindows::hWnd, uMsg, wParam, lParam);//对不感兴趣的消息进行缺省处理，必须有该代码，否则程序有问题
	}
	return 0;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,      // handle to current instance
	_In_opt_ HINSTANCE hPrevInstance,  // handle to previous instance
	_In_ LPSTR lpCmdLine,          // command line
	_In_ int nCmdShow              // show state
) {
	SetConsoleOutputCP(65001); // 设置为UTF-8编码
	CHAR run[260] = { 0 };
	GetModuleFileNameA(NULL, run, MAX_PATH);
	int len = MultiByteToWideChar(CP_ACP, 0, run, -1, NULL, 0);
	wchar_t* run_ = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, run, -1, run_, len);
	Log::wrunpath = run_;
	mywindows::log("getting runpath \n%s", run);
	mywindows::log(L"getting runpath \n%s", run_);
	mywindows::removeFileNameFromPath(run);
	Log::wrunpath = run_;
	mywindows::removeFileNameFromPath(Log::wrunpath);
	Log::runpath = run;
	mywindows::log("initing runpath %s", Log::runpath.c_str());
	mywindows::log(L"initing runpath %ws", Log::wrunpath.c_str());
	config::init();
	setting::offmusic = std::stoi(config::get(OFFMUSIC));
	WNDCLASS wndcls; //创建一个窗体类
	wndcls.cbClsExtra = 0;//类的额外内存，默认为0即可
	wndcls.cbWndExtra = 0;//窗口的额外内存，默认为0即可
	wndcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//获取画刷句柄（将返回的HGDIOBJ进行强制类型转换）
	wndcls.hCursor = LoadCursorW(NULL, IDC_ARROW);//设置光标
	wndcls.hIcon = LoadIconW(NULL, IDI_ERROR);//设置窗体左上角的图标
	wndcls.hInstance = hInstance;//设置窗体所属的应用程序实例
	wndcls.lpfnWndProc = WinSunProc;//设置窗体的回调函数，暂时没写，先设置为NULL，后面补上
	wndcls.lpszClassName = L"main";//设置窗体的类名
	wndcls.lpszMenuName = NULL;//设置窗体的菜单,没有，填NULL
	wndcls.style = CS_HREDRAW | CS_VREDRAW;//设置窗体风格为水平重画和垂直重画
	RegisterClass(&wndcls);//向操作系统注册窗体
	setting::fullscreen = !std::stoi(config::get(INWINDOW));
	if (setting::fullscreen)mywindows::hWnd = CreateWindowW(L"main", config::get(WINDOW_TITEL).c_str(), WS_POPUP | WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN, 0, 0, mywindows::windowWidth, mywindows::windowHeight, NULL, NULL, hInstance, NULL);
	else {
		mywindows::windowWidth = mywindows::screenWidth * 0.6;
		mywindows::windowHeight = mywindows::screenHeight * 0.6;
		mywindows::hWnd = CreateWindowW(L"main", config::get(WINDOW_TITEL).c_str(), WS_CLIPSIBLINGS | WS_OVERLAPPED | WS_CLIPCHILDREN | WS_THICKFRAME, 0, 0, mywindows::windowWidth, mywindows::windowHeight, NULL, NULL, hInstance, NULL);
	}
	DWORD threadId;
	// 创建线程
	paintname::random_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)paintname::RandomNumberGenerator, NULL, 0, &threadId);
	setting::offvideo = std::stoi(config::get(OFF_VIDEO));
	ui::mode = std::stoi(config::get(MODE));
	setting::offmusic = std::stoi(config::get(OFFMUSIC));
	ShowWindow(mywindows::hWnd, SW_SHOWNORMAL);//把窗体显示出来
	FILE* t;
	fopen_s(&t, ".\\version", "w");
	fprintf(t, "1.0.0");
	fclose(t);
	t = 0;
	if (std::filesystem::exists("upgrade_temp.exe"))
	{
		std::filesystem::remove("upgrade.exe");
		std::filesystem::rename("upgrade_temp.exe", "upgrade.exe");
	}
	ShellExecute(NULL, L"open", L"upgrade.exe", NULL, NULL, SW_SHOWNORMAL);
	mywindows::log("INIT COMPETLLY SUCCESSFULLY");
	MSG msg;
	while (GetMessageW(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	mywindows::log("bye!");
	return 0;
}