#include "hsvColorPicker.h"
#include "../../color.h"

BEGIN_GUI_EXT_NAMESPACE

HSVColorPicker::HSVColorPicker()
{
    _sliders[0].pos = {0, 0};
    _sliders[0].range = 359;

    _sliders[1].pos = {0, 64};
    _sliders[1].range = 1;

    _sliders[2].pos = {0, 128};
    _sliders[2].range = 1;

    for(int i = 0; i < 3; i++)
    {
        _sliders[i].style = 1;
        //_sliders[i].bordercolor = {1,1,1,1};
    }
    value = {0, 0, 0};
}

void HSVColorPicker::setValue(vec3f color)
{
    vec3f hsv = RGB2HSV(color);
    _sliders[0].value = hsv.x;
    _sliders[1].value = hsv.y;
    _sliders[2].value = hsv.z;
}

bool HSVColorPicker::update(vec2d c, uint key)
{
    bool ret = 0;
    for(auto &i:_sliders)
    {
        i._addPos = pos+_addPos;
        if(i.update(c, key))
            ret = 1;
    }

    value = hsv2RGB({_sliders[0].value, _sliders[1].value, _sliders[2].value});
    return ret;
}

bool HSVColorPicker::update(uint key)
{
    bool ret = 0;
    for(auto &i:_sliders)
    {
        i._addPos = pos+_addPos;
        if(i.update(key))
            ret = 1;
    }
    hsv = {_sliders[0].value, _sliders[1].value, _sliders[2].value};
    if(hsv != _hsv)
    {
        _sliders[0].frontcolor = hsv2RGBA(vec3f(hsv.x, 1, 1));
        _sliders[0].backcolor = hsv2RGBA(vec3f(hsv.x, 0.2, 0.2));

        _sliders[1].frontcolor = hsv2RGBA(vec3f(hsv.x, hsv.y, 1));
        _sliders[1].backcolor = _sliders[0].backcolor;

        _sliders[2].frontcolor = hsv2RGBA(vec3f(hsv.x, hsv.y, hsv.z));
        _sliders[2].backcolor = _sliders[0].backcolor;
        value = hsv2RGB(hsv);
        _hsv = hsv;
    }
    return ret;
}

void HSVColorPicker::draw()
{
    for(auto &i:_sliders)
        i.draw();
}

END_GUI_EXT_NAMESPACE
