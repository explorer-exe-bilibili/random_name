#pragma once

#include <string>
#include <random>

namespace core
{
enum class NameType {
    sword,
    polearm,
    claymore,
    catalyst,
    bow
};

// 前向声明 NameEntry 结构体
struct NameEntry;

class NameRandomer {
    static std::shared_ptr<NameRandomer> instance;
public:
    static NameRandomer* getInstance() {
        if (instance == nullptr) {
            instance = std::shared_ptr<NameRandomer>(new NameRandomer());
        }
        return instance.get();
    }
    NameRandomer();
    ~NameRandomer();

    bool setFile(const std::string& filePath);
    NameEntry GetNameEntry();
private:
    std::random_device rd;
    std::mt19937 rng{rd()};
    std::uniform_int_distribution<int> dist;
    int index;
};

struct NameEntry {
    std::string name;
    int star;
    NameType type;
    NameEntry() { *this = NameRandomer::getInstance()->GetNameEntry(); }
    NameEntry(const std::string& name, int star, NameType type)
        : name(name), star(star), type(type) {}
};
}