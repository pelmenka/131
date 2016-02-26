#ifndef GUI_OBJECT_H_INCLUDED
#define GUI_OBJECT_H_INCLUDED

#include "../../common.h"

class _guiObject
{
public:
    bool visible, enabled;
    _guiObject(){visible = 1; enabled = 1; _addPos = {0, 0};}

    virtual bool update(uint = 0) = 0;
    virtual bool update(vec2d, uint = 0) = 0;
    virtual void draw()=0;
    vec2f size, pos, _addPos;
protected:
    vec2f _size, _pos;
};

#endif // GUI_OBJECT_H_INCLUDED
