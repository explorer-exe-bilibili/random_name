#pragma once
#include<windows.h>
#include<string>
#include<vector>
#include <nlohmann/json.hpp>


class Button;
class Gp;
class SetButton;
struct sitem;

class set2
{
	struct spage {
		int ItemCount = 0;
		std::wstring Title;
		std::vector<sitem> items;
		std::vector<std::shared_ptr<SetButton>> buttons;
	};
	Gp* p;
	std::vector<spage> pages;
	std::vector<Button> buttons;
public:
	static bool offVideo, reran, offMusic, fullscreen, FloatWindow,noSmoothUI;
	int page;
	set2();
	~set2();
	explicit set2(std::string& jsonfile);
	void clicked(int x, int y) const;
	void paint() const;
	void quit() const;
	static void reinit();
	void resetplace();
	static void enter();
	void setEditBox(WPARAM wParam) const;
	void setGp(Gp* p);
	static void rereadConfig();
private:
	std::string G2U(const std::wstring& gbk);
	std::string U2G(const std::string& utf8);
	nlohmann::json rollback(const std::string& jsonpath);
	void changePage() const;
	static void init();
	void Load(const std::string& jsonpath);
	void showItem(const sitem& item) const;
	void regButton();
};
