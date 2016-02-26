#include "../gui.h"
#include "../../color.h"

BEGIN_GUI_NAMESPACE

checkbox::checkbox()
{
    flag = BUTTON_BEHAVE_SWITCH;
    size = {32, 32};
    checkColor = Hex2RGBA(0x222034);
}

bool checkbox::update(int key)
{
    return button::update(key);
}

END_GUI_NAMESPACE
