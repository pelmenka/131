#include <GLFW/glfw3.h>
#include "../gui.h"
#include "../../color.h"


BEGIN_GUI_NAMESPACE

progressBar::progressBar()
{
    range = 100;
    value = 0;
    _value = 0;

    frontcolor = vec4f(.31, .38, .77, 1);
    backcolor = vec4f(.19, .23, .46, 1);

    pos = vec2f(0, 0);
    size = vec2f(192, 48);

    behaviour = 3;
    infinite = 0;
}

bool progressBar::update(uint)
{
    return update(vec2d());
}

bool progressBar::update(vec2d, uint)
{
    if(value < 0)
        value = 0;
    else if(value > range)
        value = range;
    float delta = glfwGetTime() - _oldTime;
    _oldTime = glfwGetTime();
    delta *= size.x;
    if(_value < value)
    {
        if(behaviour&1)
        {
            if(delta > value-_value)
                _value = value;
            else
                _value += delta;
        }
        else
            _value = value;
    }
    else if(_value > value)
    {
        if(behaviour&2)
        {
            if(delta > value-value)
                _value = value;
            else
                _value -= delta;
        }
        else
            _value = value;
    }
    return 0;
}

END_GUI_NAMESPACE
