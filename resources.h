#pragma once
#include "render/image.h"
#include "render/atlas.h"
#include "render/shader.h"
#include "render/particles.h"

namespace resources
{
extern render::image tiles;
extern render::textureAtlas bullets, entities, guns;
extern render::effect2D bloodEffect, coolEffect;

void init();
}
