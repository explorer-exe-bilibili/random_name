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

struct NameEntry {
    std::string name;
    int star;
    NameType type;
};

class NameRandomer {
public:
    NameRandomer();
    ~NameRandomer();

    bool setFile(const std::string& filePath);

    std::string GetRandomName();
    NameType GetNameType();
    int GetStar();
    NameEntry GetNameEntry();
private:
    std::random_device rd;
    std::mt19937 rng{rd()};
    std::uniform_int_distribution<int> dist;
    int index;
};
}