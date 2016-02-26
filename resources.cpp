#include <tinyxml2.h>
#include "resources.h"
#include "color.h"

using namespace tinyxml2;

namespace resources
{

render::image games, icons, arrows;

void init()
{
    games.load("data/textures/gameList.png");
    games.parts = {2, 2};
    icons.load("data/textures/icons.png");
    icons.parts = {3, 1};
    arrows.load("data/textures/arrows.png");
    arrows.parts = {2, 1};
}

void free()
{

}

}
