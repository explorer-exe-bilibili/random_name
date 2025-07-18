#pragma once
#include <vector>
#include <atomic>
#include <mutex>

#include "base.h"
#include "../nameRandomer.h"

namespace screen
{
class NameRegion:public core::Region{
    core::Region fatherRegion;
public:
    float x,xend,y,yend;
    
    // 构造函数
    NameRegion() : core::Region(), x(0), y(0), xend(0), yend(0) {}
    NameRegion(double x, double y, double xend, double yend) 
        : core::Region(x, y, xend, yend), x(x), y(y), xend(xend), yend(yend) {}
    
    void setFatherRegion(core::Region& father){fatherRegion=father;}
    float getx() const ;
    float gety() const ;
    float getxend() const;
    float getyend() const ;
};


class NameButton:public core::Button {
    std::vector<NameRegion> regions;
    std::wstring text;
    int starCount=0;
    static core::Color color;
    static core::Color star6Color;
public:
    void Draw(unsigned char alpha=255);
    void SetName(const core::NameEntry& name);
    static void setColor(const core::Color& newColor){color=newColor;}
    static void set6Color(const core::Color& newColor){star6Color=newColor;}
};
enum RegionState{
    NameAppear,
    TypeAppear,
    Complete
};
class NameScreen : public Screen {
    core::Font** StarFont;
    core::Font** NameFont;
    static core::Color SmallNameColor;
    unsigned char alpha=0;
    mutable std::mt19937 rng;
    mutable std::uniform_real_distribution<float> dist;
    mutable std::vector<core::Point> starPositions;
    mutable std::mutex starPositionsMutex;
    std::vector<core::Region> regions;
    core::NameEntry currentName;
    int currentIndex = 0;
    std::shared_ptr<NameButton> nameButton;
    int currentRegionState=0;
    void reloadButtonsRegion() override;
public:
    NameScreen():Screen(ScreenID::Name){init();}
    void Draw() override;
    void init() override;
    void enter(int mode) override;
    bool Click(int x, int y)override;
    static void setSmallNameColor(const core::Color& color) { SmallNameColor = color; }
    void setRegionState(RegionState mode);
private:
    void PaintStars() const;
    void changeName();
};
}