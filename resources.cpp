#include <tinyxml2.h>
#include "resources.h"
#include "color.h"

using namespace tinyxml2;

#define TEXTURE_PATH "data/textures/"

namespace resources
{

render::image tiles;
render::textureAtlas bullets, entities, guns;
render::effect2D bloodEffect, coolEffect;

void init()
{
    tiles.load(TEXTURE_PATH"tiles.png");
    tiles.parts = vec2u(5, 1);
    entities.load(TEXTURE_PATH"actors.atlas", 1);
    bullets.load(TEXTURE_PATH"bullet.atlas", 1);
    guns.load(TEXTURE_PATH"weapon.atlas");
    bloodEffect.load("data/fx/blood.xml");
    coolEffect.load("data/fx/cooler.xml");
}

}
