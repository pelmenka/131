#include "panel.h"

BEGIN_GUI_NAMESPACE

panel::~panel()
{
    for(auto i: _objects)
        delete i;
    _objects.clear();
}

void panel::add(_guiObject *obj)
{
    _objects.push_back(obj);
}

void panel::remove(_guiObject *obj)
{
    if(_objects.empty())
        return;
    delete obj;
    _objects.remove(obj);
}

bool panel::update(uint key)
{
    if(_objects.empty() || !enabled)
        return 0;

    for(auto i: _objects)
    {
        i->_addPos = pos;
        i->update(key);
    }

    return 0;
}

bool panel::update(vec2d cur, uint key)
{
    if(_objects.empty() || !enabled)
        return 0;

    for(auto i: _objects)
    {
        i->_addPos = pos;
        i->update(cur, key);
    }

    return 0;
}


void panel::draw()
{
    if(visible)
        for(auto i: _objects)
            i->draw();
}


END_GUI_NAMESPACE
