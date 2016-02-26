#pragma once
#include <string>
#include "common.h"

namespace window
{
    void createWindow(const vec2i &size, const std::string &name);
    void swapBuffers();
    void closeWindow();
    vec2i getSize();
    bool shouldClose();
}
