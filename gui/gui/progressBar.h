#ifndef _GUI_GAUGE_H_INCLUDED
#define _GUI_GAUGE_H_INCLUDED

#include "../../common.h"

#include "../gui.h"
#include "object.h"

#define GUI_PROGRESSBAR_NO_SMOOTH 0
#define GUI_PROGRESSBAR_FORWARD_SMOOTH 1
#define GUI_PROGRESSBAR_BACKWARD_SMOOTH 2
#define GUI_PROGRESSBAR_SMOOTH 3

#define GUI_PROGRESSBAR_LAYOUT_HORISONTAL 0
#define GUI_PROGRESSBAR_LAYOUT_VERTICAL 1

BEGIN_GUI_NAMESPACE

class progressBar: public _guiObject
{
public:
    bool infinite;

    uint behaviour;
    bool layout;

    vec4f backcolor, frontcolor;
    float value, range;

    progressBar();

    virtual bool update(vec2d, uint = 0);
    virtual bool update(uint = 0);
    virtual void draw();
protected:
    float _value, _oldTime;
};


END_GUI_NAMESPACE
#endif // _GUI_GAUGE_H_INCLUDED
