#pragma once

// 这个文件定义了应用程序中使用的音频ID常量
// 使用这些常量而不是直接使用字符串，可以避免拼写错误

namespace audio {

// 背景音乐
constexpr const char* BGM = "bgm";
constexpr const char* MENU_BGM = "menu_bgm";
constexpr const char* GAME_BGM = "game_bgm";

// 界面音效
constexpr const char* CLICK = "click";
constexpr const char* ENTER = "enter";
constexpr const char* BACK = "back";
constexpr const char* ERROR = "error";
constexpr const char* SUCCESS = "success";

// 游戏音效
constexpr const char* START = "start";
constexpr const char* WIN = "win";
constexpr const char* LOSE = "lose";

} // namespace audio
