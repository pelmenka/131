#include <tinyxml2.h>
#include "resources.h"
#include "color.h"

using namespace tinyxml2;

#define TEXTURES_PATH "data/textures/"

namespace resources
{

render::image hero;
render::textureAtlas bullets;

void init()
{
    hero.load(TEXTURES_PATH"hero.png");
    hero.center = vec2d(.5);
    bullets.load("data/textures/bullet.atlas", 1);
}

void free()
{

}

}
