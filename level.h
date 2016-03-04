#pragma once

#include "common.h"
#include "hero.h"

namespace level
{
    void makeShot(vec2f, float, float, float);

    void init();
    void update(hero*);
    void draw();
}
