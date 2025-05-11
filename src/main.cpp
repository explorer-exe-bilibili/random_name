#include "mainloop.h"

using namespace std;

int main()
{
    if(init()!=0){
        Log<<Level::Error<<"Failed to initialize"<<op::endl;
        return -1;
    }
    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        mainloop(window);
    }
    cleanup();
    return 0;
}
