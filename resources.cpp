#include <tinyxml2.h>
#include "resources.h"
#include "color.h"

using namespace tinyxml2;

#define TEXTURES_PATH "data/textures/"

namespace resources
{

render::image hero;
render::textureAtlas bullets, entities;
render::effect2D bloodEffect;

void init()
{
    entities.load(TEXTURES_PATH"111.atlas", 1);
    hero.load(TEXTURES_PATH"hero.png");
    hero.center = vec2d(.5);
    bullets.load("data/textures/bullet.atlas", 1);
    bloodEffect.load("data/fx/blood.xml");
}

void free()
{

}

}
