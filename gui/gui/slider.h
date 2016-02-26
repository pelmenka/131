#ifndef _GUI_SLIDER_H_INCLUDED
#define _GUI_SLIDER_H_INCLUDED

#include "progressBar.h"

BEGIN_GUI_NAMESPACE

class slider:public progressBar
{
public:
    slider();
    bool style;
    bool update(uint = 0);
    bool update(vec2d, uint = 0);
    virtual void draw();
};

END_GUI_NAMESPACE

#endif // _GUI_SLIDER_H_INCLUDED
