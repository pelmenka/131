#ifndef _GUI_CHECKBOX_H_INCLUDED
#define _GUI_CHECKBOX_H_INCLUDED

#include "button.h"

namespace gui{

class checkbox: public button
{
public:
    vec4f checkColor;

    bool value;
    checkbox();

    bool update(int = 0);
    void draw();
};

}

#endif // _GUI_CHECKBOX_H_INCLUDED
