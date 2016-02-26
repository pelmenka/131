#ifndef _GUI_TEXTBOX_H
#define _GUI_TEXTBOX_H

#include "../gui.h"

BEGIN_GUI_NAMESPACE

class textBox: public button
{
public:
    bool multiline;
    uint maxSize;

    textBox();

    bool update(vec2d, uint = 0);
    bool update(uint = 0);
    void draw();
private:
    int position;
};

END_GUI_NAMESPACE

#endif // _GUI_TEXTBOX_H
