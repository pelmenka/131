#include <cmath>
#include "render.h"
#include "image.h"
#include "type.h"

#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923

namespace render{

image::image():scale(1),color(1),parts(1)
{
    angle = 0;
}

image::image(const std::string& path, bool smooth, bool mipmap):image()
{
    load(path, smooth, mipmap);
}

bool image::load(const std::string& path, bool smooth, bool mipmap)
{
    if(texture::load(path, smooth, mipmap))
    {
        _mathRotate();
        return 1;
    }
    return 0;
}

void image::create(const vec2u& size, bool smooth)
{
    texture::create(size, smooth);
    _size = size;
    _mathRotate();
}

image::~image()
{
    free();
}

void image::draw(const vec2f &pos) noexcept
{
    if(!_size.y) return;
    CHECK_DEF_SHAD(regular);
    _checkParams();
    const vec2f offset[6] = {vec2f(0), vec2f(1, 0), vec2f(1), vec2f(1), vec2f(0, 1), vec2f(0)};

    bind();
    type2D::texVert data[6];
    for(int i = 0; i < 6; i++)
    {
        data[i].vert = pos+_rotMatr[i];
        data[i].texCoord = offset[i];
        data[i].color = color;
    }

    render::_internal::_addTriangles2D(data, 6);
}

void image::draw(const vec2f &pos, const vec2u &part) noexcept
{
    if(!_size.y) return;
    CHECK_DEF_SHAD(regular);
    _checkParams();

    const vec2f offset[6] = {vec2f(0, 1), vec2f(1), vec2f(1, 0), vec2f(1, 0), vec2f(0), vec2f(0, 1)};

    bind();
    type2D::texVert data[6];
    for(int i = 0; i < 6; i++)
    {
        data[i].vert = pos+_rotMatr[i];
        data[i].texCoord = (offset[i]+part)/parts;
        data[i].texCoord.y = 1.0-data[i].texCoord.y;
        data[i].color = color;
    }
    render::_internal::_addTriangles2D(data, 6);
}

void image::_checkParams()
{
    bool needMath = 0;
    if(parts != _oldparts)
    {
        if(parts.x < 1) parts.x = 1;
        if(parts.y < 1) parts.y = 1;
        _oldparts = parts;
        needMath = 1;
    }

    if(center != _oldcenter)
    {
        _oldcenter = center;
        needMath = 1;
    }

    if(angle != _oldangle)
    {
        angle = fmod(angle, M_PI*2);
        _oldangle = angle;
        needMath = 1;
    }

    if(scale != _oldscale)
    {
        _oldscale = scale;
        needMath = 1;
    }

    if(needMath)
        _mathRotate();
}

void image::_mathRotate()
{
    vec2f offset[6] = {vec2f(0, 1), vec2f(1), vec2f(1, 0), vec2f(1, 0), vec2f(0), vec2f(0, 1)};
    float msin, mcos;

    sincosf(angle, &msin, &mcos);
    for(int i = 0; i < 6; i++)
    {
        offset[i] -= center;
        offset[i] *= _size/parts*scale;

        _rotMatr[i].x = offset[i].x*mcos-offset[i].y*msin;
        _rotMatr[i].y = offset[i].x*msin+offset[i].y*mcos;
    }
}

void image::copyScreen(const vec2i& position, const vec2u& size)
{
    texture::copyScreen(position, size);
    _mathRotate();
}
}

