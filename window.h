#pragma once
#include <string>
#include "common.h"

namespace window
{
    extern double deltaTime;

    void createWindow(const vec2i &size, const std::string &name);
    void swapBuffers();
    void closeWindow();
    vec2i getSize();
    bool shouldClose();
}
