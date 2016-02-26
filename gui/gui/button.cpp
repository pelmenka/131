#include "../../input.h"
#include <GLFW/glfw3.h>
#include "button.h"
#include "../../color.h"

namespace gui{

button::button()
{
    flag = 0;
    state = 0;
    size = {144, 48};
    pos = {0, 0};
    _size = size;
    _pos = pos;

    label.color = vec4f(0, 0, 0, 1);

    regularColor = Hex2RGBA(0xFFFFFF);
    hoverColor   = Hex2RGBA(0xF4FDFF);
    activeColor  = Hex2RGBA(0xEFF8F9);

    actionHold = 0;
    _smoothState = 1;
}

bool button::update(uint key)
{
    return button::update(input::getCursor(), key);
}

bool button::update(vec2d coord, uint key)
{
    float delta = glfwGetTime() - _oldTime;
    _oldTime = glfwGetTime();
    delta *= 3;
    bool act = input::getKey(key);
    if(!act)
    {
        actionHold = false;
        if(!flag) state = 0;
        else state &= 2;
    }

    bool hover = false;
    if(coord.x >= pos.x+_addPos.x
    && coord.y >= pos.y+_addPos.y
    && coord.x <  pos.x+size.x+_addPos.x
    && coord.y <  pos.y+size.y+_addPos.y) hover = true;

    state |= hover;

    if(act && !actionHold)
    {
        if(hover)
        {
            if(flag&BUTTON_BEHAVE_SWITCH) state ^= 2;
            else state |= 2;
        }
        else
        {
            if(flag&BUTTON_BEHAVE_LOSE_STATE)
                state &= 1;
        }
        actionHold = true;
    }

    float target;

    switch(state)
    {
    case 0:
        target = 1; break;
    case 1:
        target = 0.5; break;
    default:
        target = 0; break;
    }

    if(_smoothState < target)
    {
        if(delta > target-_smoothState)
            _smoothState = target;
        else
            _smoothState += delta;
    }else if(_smoothState > target)
    {
        if(delta > _smoothState-target)
            _smoothState = target;
        else
            _smoothState -= delta;
    }


    return (state&2);
}

}
