#include "../gui.h"
#include "../../input.h"

BEGIN_GUI_NAMESPACE

slider::slider()
{
    style = 0;
}

bool slider::update(uint key)
{
    return update(input::getCursor(), key);
}

bool slider::update(vec2d coord, uint key)
{
    static bool actHold = 0;
    bool act = input::getKey(key);
    //progressBar::update();
    static vec2d oldCur = {0, 0};
    if(!act)
    {
        actHold = false;
        return 0;
    }
    else if(act && !actHold)
    {
        oldCur = coord;
        actHold = true;
    }
    if(oldCur.x < pos.x+_addPos.x
    || oldCur.y < pos.y+_addPos.y
    || oldCur.x > pos.x+size.x+_addPos.x
    || oldCur.y > pos.y+size.y+_addPos.y) return 0;

    coord.x -= pos.x+_addPos.x;



    if(coord.x < 0)
        coord.x = 0;
    else if(coord.x > size.x)
        coord.x = size.x;
    value = coord.x/size.x*range;

    return 1;
}

void slider::draw()
{
    progressBar::draw();
    /*
    if(!style)

    else
    {
        gauge::update();
        render::primitives2d::fillColor = backcolor;
        render::primitives2d::quad(pos+_addPos, {size.x, size.y});

        render::primitives2d::fillColor = frontcolor;
        float a = size.x/12.0;
        float s = (size.x*value/range)-a/2;

        if(s+a > size.x)
            a = size.x-s;
        if(s < 0)
        {
            a += s;
            s = 0;
        }


        render::primitives2d::quad({pos.x+_addPos.x+s, pos.y+_addPos.y}, {a, size.y});
        render::primitives2d::rect(pos+_addPos, size, bordercolor);
    }*/
}

END_GUI_NAMESPACE
