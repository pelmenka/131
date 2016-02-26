#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#include "../gui.h"
#include "object.h"
#include <list>

BEGIN_GUI_NAMESPACE

class panel: public _guiObject
{
public:
    ~panel();
    void add(_guiObject*);
    void remove(_guiObject*);

    bool update(uint = 0);
    bool update(vec2d, uint = 0);
    void draw();

    std::list<_guiObject*>::iterator begin();
    std::list<_guiObject*>::iterator end();
    _guiObject& operator[](uint);
private:
    std::list<_guiObject*> _objects;
};

END_GUI_NAMESPACE

#endif // PANEL_H_INCLUDED
