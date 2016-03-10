#pragma once
#include "render/image.h"
#include "render/atlas.h"
#include "render/shader.h"
#include "render/particles.h"

namespace resources
{
extern render::image hero;
extern render::textureAtlas bullets, entities;
extern render::effect2D bloodEffect;

void init();
void free();
}
