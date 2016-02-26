#pragma once
#include "common.h"

namespace input
{
    uchar getKey(uint);
    const vec2d &getCursor();
    float getScroll();
    uint getSymbol();

    void setKey(uint, uchar);
    void setCursor(vec2d);
}

