#ifndef PLMBUTTON_H_INCLUDED
#define PLMBUTTON_H_INCLUDED

#include "object.h"
#include "../../render/text.h"
#include "../../render/atlas.h"

#define BUTTON_BEHAVE_SWITCH     1
#define BUTTON_BEHAVE_ACTIVATE   2
#define BUTTON_BEHAVE_LOSE_STATE 4

namespace gui{

class button: public _guiObject
{
public:
    vec4f regularColor,
          hoverColor,
          activeColor;

    int flag;
    char state;

    render::text label;

    render::icon icon;

    button();
    virtual bool update(uint = 0);
    virtual bool update(vec2d, uint = 0);
    virtual void draw();
protected:
    float _oldTime, _smoothState;
    bool actionHold;
};

}
#endif // PLMBUTTON_H_INCLUDED
