#include "mainloop.h"

using namespace std;

int main()
{
#ifdef DEBUG_MODE
    Log<<Level::Info<<"运行在Debug模式"<<op::endl;
#else
    Log<<Level::Info<<"运行在非Debug模式"<<op::endl;
#endif
    if(init()!=0){
        Log<<Level::Error<<"Failed to initialize"<<op::endl;
        return -1;
    }
    // 渲染循环
    while (!glfwWindowShouldClose(core::screenInfo.window)) {
        mainloop();
    }
    cleanup();
    return 0;
}
