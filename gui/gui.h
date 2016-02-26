#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

namespace gui
{
    class checkBox;
    class progressBar;
    class slider;
    class textBox;
    class panel;
    namespace ext
    {
        class HSVColorPicker;
        class RGBColorPicker;
    }
}

#define BEGIN_GUI_NAMESPACE namespace gui{
#define END_GUI_NAMESPACE }

#define BEGIN_GUI_EXT_NAMESPACE namespace gui{namespace ext{
#define END_GUI_EXT_NAMESPACE }}

#include "gui/progressBar.h"
#include "gui/checkbox.h"
#include "gui/textBox.h"
#include "gui/slider.h"
#include "gui/panel.h"
#include "ext/HSVColorPicker.h"

#endif // GUI_H_INCLUDED
