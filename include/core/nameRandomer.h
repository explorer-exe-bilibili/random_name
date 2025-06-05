#pragma once

#include <string>
#include <random>
#include <vector>

namespace core
{
enum class NameType {
    sword,
    polearm,
    claymore,
    catalyst,
    bow,
    Unknow
};

// 前向声明 NameEntry 结构体
struct NameEntry;

class NameRandomer {
    static std::vector<std::shared_ptr<NameRandomer>> instances;
    NameRandomer(int index);
    static std::shared_ptr<NameRandomer> createInstance(int index);
public:
    static NameRandomer* getInstance(int mode);
    ~NameRandomer();

    bool setFile(const std::string& filePath);
    bool addName(NameEntry entry);
    NameEntry GetNameEntry();
private:
    std::random_device rd;
    std::mt19937 rng{rd()};
    std::uniform_int_distribution<int> dist;
    int index;
    std::vector<NameEntry> entries;
};

struct NameEntry {
    std::string name;
    int star=0;
    NameType type=NameType::Unknow;
    int mode=0;

    NameEntry() = default;
    NameEntry(const std::string& name, int star, NameType type,int mode)
        : name(name), star(star), type(type), mode(mode) {}
};
}