#pragma once
#include"Gp.h"
class GpManager {
public:
    static Gp& getInstance() {
        static Gp instance;
        return instance;
    }

    static void reset() {
        Gp temp; // 创建一个新的 Gp 实例
        GpManager::instance = std::move(temp); // 使用 std::move 来转移所有权
    }
private:
    static Gp instance;
};