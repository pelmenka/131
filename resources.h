#ifndef RESOURCES_H_INCLUDED
#define RESOURCES_H_INCLUDED

#include <string>
#include "render/image.h"
#include "render/shader.h"

namespace resources
{
extern render::image games, icons, arrows;
void init();
void free();
}

#endif // RESOURCES_H_INCLUDED
