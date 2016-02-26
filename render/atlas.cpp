#include "atlas.h"
#include <math.h>
#include <tinyxml2.h>
#include <list>
#include "../log.h"

using namespace tinyxml2;

namespace render{

textureAtlas::textureAtlas()
{

}

textureAtlas::textureAtlas(const std::string &path, bool smooth):textureAtlas()
{
    load(path, smooth);
}

textureAtlas::~textureAtlas()
{
    free();
}

const vec4f &textureAtlas::getTexCoord(uint c) const
{
    if(c >= elements.size())
        return vec4f(0, 0, 1, 1);
    return vec4f(elements[c].pos/_size, elements[c].size/_size);
}

#define CheckAttribute(x, y, z) \
        if(z->QueryAttribute(x, y))\
            _log::out("[WARNING] Can't find attribute %s", x);


bool textureAtlas::load(const std::string &path, bool smooth)
{
    _log::out("Loading texture atlas : %s\n", path.c_str());
    XMLDocument document;

    if(document.LoadFile(path.c_str()))
        return 0;
    free();

    XMLElement *element = document.FirstChildElement("data");
    uint elCount = 0;
    element->QueryAttribute("size", &elCount);
    elements.reserve(elCount);

    if(!texture::load(path.substr(0,path.rfind('/')+1)+element->Attribute("source"), smooth))
        return nullptr;

    element = element->FirstChildElement("sprite");

    for(int i = 0; i < elCount && element; i++, element = element->NextSiblingElement())
    {
        textureAtlasNode temp;
        temp.angle = 0;
        temp.scale = vec2f(1);
        temp.color = rgba(1);

        CheckAttribute("x", &temp.pos.x, element);
        CheckAttribute("y", &temp.pos.y, element);
        CheckAttribute("w", &temp.size.x, element);
        CheckAttribute("h", &temp.size.y, element);
        CheckAttribute("pX", &temp.center.x, element);
        CheckAttribute("pY", &temp.center.y, element);
        temp.size += vec2f(1);
        elements.push_back(temp);
    }
    return 1;
}

void textureAtlas::mathRotate(textureAtlasNode &p)
{
    vec2f offset[6] = {{0, 1}, {1, 1}, {1, 0},
                       {1, 0}, {0, 0}, {0, 1}};
    float msin, mcos;
    sincosf(p.angle, &msin, &mcos);

    for(int i = 0; i < 6; i++)
    {
        offset[i] -= p.center;
        offset[i] *= p.size;

        p.rotMatrix[i].x = offset[i].x*mcos-offset[i].y*msin;
        p.rotMatrix[i].y = offset[i].x*msin+offset[i].y*mcos;
    }
}

void textureAtlas::mathRotate(icon &p)
{
    vec2f offset[6] = {{0, 1}, {1, 1}, {1, 0},
                       {1, 0}, {0, 0}, {0, 1}};
    float msin, mcos;
    sincosf(p.angle, &msin, &mcos);

    for(int i = 0; i < 6; i++)
    {
        offset[i] -= p.center;
        offset[i] *= p.size;

        p.rotMatrix[i].x = offset[i].x*mcos-offset[i].y*msin;
        p.rotMatrix[i].y = offset[i].x*msin+offset[i].y*mcos;
    }
}

void textureAtlas::draw(const vec2f&pos, uint p)
{
    if(p >= elements.size())
        return;
    const vec2f offset[6] = {{0, 1}, {1, 1}, {1, 0},
                             {1, 0}, {0, 0}, {0, 1}};
    if(elements[p].angle != elements[p]._angle || elements[p].center != elements[p]._center)
    {
        mathRotate(elements[p]);
        elements[p]._angle = elements[p].angle;
        elements[p]._center = elements[p].center;
    }
    type2D::texVert data[6];
    for(int i = 0; i < 6; i++)
    {
        data[i].vert = pos+elements[p].rotMatrix[i]*elements[p].scale;
        data[i].texCoord.x = float(elements[p].pos.x+offset[i].x*elements[p].size.x)/_size.x;
        data[i].texCoord.y = 1.0-float(elements[p].pos.y+offset[i].y*elements[p].size.y)/_size.y;
        data[i].color = elements[p].color;
    }
    CHECK_DEF_SHAD(regular);
    bind();
    render::_internal::_addTriangles2D(data, 6);
}

void textureAtlas::draw(const vec2f &pos, icon &p)
{
    const vec2f offset[6] = {{0, 1}, {1, 1}, {1, 0},
                             {1, 0}, {0, 0}, {0, 1}};
    if(p._parent != this)
        return;
    if(p.angle != p._angle || p.center != p._center)
    {
        mathRotate(p);
        p._angle = p.angle;
        p._center = p.center;
    }
    type2D::texVert data[6];
    for(int i = 0; i < 6; i++)
    {
        data[i].vert = pos+p.rotMatrix[i]*p.scale;
        data[i].texCoord.x = float(p.pos.x+offset[i].x*p.size.x)/_size.x;
        data[i].texCoord.y = 1.0-float(p.pos.y+offset[i].y*p.size.y)/_size.y;
        data[i].color = p.color;
    }
    CHECK_DEF_SHAD(regular);
    bind();
    render::_internal::_addTriangles2D(data, 6);
}

icon textureAtlas::getIcon(uint c)
{
    if(c >= elements.size())
        c = 0;
    icon temp;
    memcpy(&temp, &elements[c], sizeof(textureAtlasNode));
    temp._parent = this;
    return temp;
}

void icon::draw(const vec2f &pos)
{
    if(!_parent)
        return;
    _parent->draw(pos, *this);
}

const vec2f &icon::getSize() const
{
    return textureAtlasNode::size;
}

}
