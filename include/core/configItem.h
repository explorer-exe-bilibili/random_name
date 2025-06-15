#pragma once 

#define POOL_COUNT "pool_count"
#define MODE "mode"
#define SPECIAL "special"
#define EXCHANGE_SPEED "exchange_speed"
#define NAMES1 "names1"
#define NAMES2 "names2"
#define NAMES3 "names3"
#define NAMES4 "names4"
#define OVERLAY1 "overlay1"
#define OVERLAY2 "overlay2"
#define OVERLAY3 "overlay3"
#define OVERLAY4 "overlay4"

#define NAME_COLOR "name_color"
#define NAME_COLOR_6_STAR "name_color_6_star"
#define TEXT_COLOR "text_color"

#define OFF_MUSIC "off_music"
#define INWINDOW "in_window"
#define OFF_VIDEO "off_video"
#define USE_VIDEO_BACKGROUND "use_video_background"
#define NOSMOOTHUI "no_smooth_ui"

#define FLOATWINDOW "float_window_switch"
#define FLOAT_WINDOW_WIDTH "float_window_width"
#define FLOAT_WINDOW_HEIGHT "float_window_height"
#define FLOAT_WINDOW_X "float_window_x"
#define FLOAT_WINDOW_Y "float_window_y"
#define FLOAT_WINDOW_MU "float_window_mu"
#define FLOAT_WINDOW_IMG "float_window_img"
#define FLOAT_WINDOW_ALPHA "float_window_alpha"

#define WINDOW_WIDTH "window_width"
#define WINDOW_HEIGHT "window_height"
#define WINDOW_X "window_x"
#define WINDOW_Y "window_y"
#define VERTICAL_SYNC "vertical_sync"
#define WINDOW_TITLE "window_title"
#define USE_FONT_COMPATIBILITY "font_compatibility"
#define NO_VIDEO_PRELOAD "no_video_preload"

#define BACKGROUND_IMG_PATH "background_img_path"
#define IMGS_PATH "imgs_path"
#define VIDEO_BACKGROUND_PATH "video_background_path"
#define DEFAULT_FONT_PATH "default_font_path"
#define ICON_FONT_PATH "icon_font_path"
#define NAME_FONT_PATH "name_font_path"
#define TEXT_FONT_PATH "text_font_path"
#define VIDEO_BACKGROUND_PATH "video_background_path"
#define SIGNAL_3_STAR_VIDEO_PATH "signal_3_star_video_path"
#define SIGNAL_4_STAR_VIDEO_PATH "signal_4_star_video_path"
#define SIGNAL_5_STAR_VIDEO_PATH "signal_5_star_video_path"
#define GROUP_4_STAR_VIDEO_PATH "group_4_star_video_path"
#define GROUP_5_STAR_VIDEO_PATH "group_5_star_video_path"
#define CLICK_MUSIC_PATH "click_music_path"
#define ENTER_MUSIC_PATH "enter_music_path"
#define BACKGROUND_MUSIC_PATH "background_music_path"
#define BGM_PATH BACKGROUND_MUSIC_PATH

#define DEBUG "debug"
#define SHOW_FPS "show_fps"
#define USE_JSON_SETTINGS "use_json_settings"
#define VOLUME "volume"

enum class boolconfig {
    offmusic = 0,
    inwindow = 1,
    off_video = 2,
    use_video_background = 3,
    nosmoothui = 4,
    floatwindow = 5,
    use_font_compatibility = 6,
    no_video_preload = 7,
    debug = 8,
    show_fps = 9,
    use_json_settings = 10
};

extern std::map<boolconfig, bool> bools;

// 从配置名字符串映射到boolconfig枚举
boolconfig GetBoolConfigFromString(const std::string& configName);

// 声明bools相关函数
void LoadBoolsFromConfig();
void SyncBoolsToConfig();