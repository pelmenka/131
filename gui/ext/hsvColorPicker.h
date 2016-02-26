#ifndef _GUI_EXT_HSVCOLORPICKER_H_INCLUDED
#define _GUI_EXT_HSVCOLORPICKER_H_INCLUDED

#include "../gui.h"
#include "../../common.h"

BEGIN_GUI_EXT_NAMESPACE

class HSVColorPicker: public _guiObject
{
public:
    HSVColorPicker();

    vec3f value;
    vec3f hsv;

    bool update(uint = 0);
    bool update(vec2d, uint = 0);
    void draw();
    void setValue(vec3f);
private:
    gui::slider _sliders[3];
    vec3f _hsv;
};


END_GUI_EXT_NAMESPACE

#endif // _GUI_EXT_HSVCOLORPICKER_H_INCLUDED
