#pragma once

#include "common.h"
#include "hero.h"

extern hero *actor;
extern double deltaMultiplier;

const uint arenaX = 50;
const uint arenaY = 40;

namespace level
{
    void makeShot(vec2f, float, float, float, float);

    void init();
    void close();
    void update();
    void draw();
}
