#include "../gui.h"
#include "../../render/render.h"

BEGIN_GUI_NAMESPACE

void progressBar::draw()
{
    //progressBar::update();
    render::primitives2d::drawGui(pos, 1, size, backcolor);
    render::primitives2d::drawGui(pos, 1, size*vec2f(value/range, 1), frontcolor);
}

void button::draw()
{
    vec4f color;

    if(_smoothState > 0.5)
        color = mix(regularColor, hoverColor, (_smoothState-0.5)*2);
    else
        color = mix(hoverColor, activeColor, _smoothState*2);

    render::primitives2d::drawGui(pos, _smoothState, size, color);
    label.draw(pos+size/2-label.rect/2/*+vec2f(icon.size().x, 0)/2*/);
//    icon.draw(pos+size/2-icon.size()/2-vec2f(label.rect.x, 0)/2);
}

void checkbox::draw()
{
    vec4f color;

    if(_smoothState > 0.5)
        color = mix(regularColor, hoverColor, (_smoothState-0.5)*2);
    else
        color = mix(hoverColor, activeColor, _smoothState*2);

    render::primitives2d::drawGui(pos, _smoothState, size, color);

    if(state&2)
    {
        vec2f offset;
        offset.x = (int(size.x/6))-0.5;
        offset.y = (int(size.y/6))-0.5;
        render::primitives2d::drawGui(pos+offset, 1, size-(offset*2), checkColor);
    }
}

END_GUI_NAMESPACE

