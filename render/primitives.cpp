#include <GL/glew.h>
#include <cmath>
#include "render.h"
#include "primitives.h"
#include "buffer.h"
#include "../window.h"
#include "../color.h"

const uint  vertexBufferSize = 0xfff;
const uint texVertBufferSize = 0xfff*3;

uint pointCount, lineCount, triangleCount;

render::vertexArray  VAO[3];
render::vertexBuffer VBO[3];

BEGIN_PRIMITIVES2D_NAMESPACE

rgba pointColor= {1, 1, 1, 1},
     lineColor = {1, 1, 1, 1},
     fillColor = {1, 1, 1, 1};

/*******************************************
* Простые примитивы
*
* Точки, линии, треугольники
********************************************/

void point(const vec2f &pos, const rgba &color, float width)
{
    type2D::vertex t = {vec3f(pos, width), color};
    render::_internal::_addPoints2D(&t, 1);
}

void line(const vec2f &p1, const vec2f &p2, const rgba &c)
{
    line(p1, p2, c, c);
}

void line(const vec2f &p1, const vec2f &p2, const rgba &c1, const rgba &c2)
{
    const shader *shad = render::getActiveShader();
    if(!shad
    || shad->type != shader::shaderType::primitives
    || shad->standart)
        render::defaultResources::primitivesShader.bind();
    type2D::vertex t[2] = {{vec3f(p1, 0), c1}, {vec3f(p2, 0), c2}};
    render::_internal::_addLines2D(t, 2);
}

//да, я знаю что это кусок говна
void triangle(const vec2f &p1, const vec2f &p2, const vec2f &p3,
              const vec2f &t1, const vec2f &t2, const vec2f &t3,
              const rgba &c1, const rgba &c2, const rgba &c3)
{
    const shader *shad = render::getActiveShader();
    if(!shad
    || shad->type != shader::shaderType::primitives
    || shad->standart)
        render::defaultResources::primitivesShader.bind();
    type2D::texVert t[3] = {{vec2f(p1), t1, c1}, {vec2f(p2), t2, c2}, {vec2f(p3), t3, c3}};
    render::_internal::_addTriangles2D(t, 3);
}

/*******************************************
* Сложные примитивы
*
* Квадраты, круги, окружности, кривые, и т.д.
********************************************/

//ну и говно
void quad(const vec2f &pos, const vec2f &size, const vec2f &tex1, const vec2f &tex2, const rgba &color)
{
    const vec2f offset[6] = {{0, 1}, {1, 1}, {1, 0},
                       {1, 0}, {0, 0}, {0, 1}};

    for(int i = 0; i < 2; i++)
        triangle(pos+offset[i*3]*size, pos+offset[i*3+1]*size, pos+offset[i*3+2]*size,
                 tex1+offset[i*3]*tex2, tex1+offset[i*3+1]*tex2, tex1+offset[i*3+2]*tex2,
                 color, color, color);
}

void circle(const vec2f &pos, float length, const rgba &color, float angle, uint samples)
{
    if(!samples)
        samples = length*3.1415;
    fillColor = color;
    vec2f temp = vec2f(cos(angle*DTOR)*length+pos.x, sin(angle*DTOR)*length+pos.y);
    for(int i = 1; i <= samples; i++)
    {
        vec2f result;

        /******************************
        * Костыль для устранения артефакта неполного рисования круга при angle отличном от 0
        *******************************/
        if(i == samples)
            result = vec2f(cos(angle*DTOR)*length+pos.x, sin(angle*DTOR)*length+pos.y);
        else
        {
            result.x = cos((i*360.0/samples+angle)*DTOR)*length+pos.x;
            result.y = sin((i*360.0/samples+angle)*DTOR)*length+pos.y;
        }


        triangle(temp, pos, result);
        temp = result;
    }
}

void round(const vec2f &pos, float length, const rgba &color, float angle, uint samples)
{
    if(!samples)
        samples = length*3.1415;

    vec2f temp = vec2f(cos(angle*DTOR)*length+pos.x, sin(angle*DTOR)*length+pos.y);
    for(int i = 1; i <= samples; i++)
    {
        vec2f result;
        result.x = cos((i*360.0/samples+angle)*DTOR)*length+pos.x;
        result.y = sin((i*360.0/samples+angle)*DTOR)*length+pos.y;
        line(temp, result, color);
        temp = result;
    }
}

void rect(const vec2f &pos, const vec2f &size, const rgba &color)
{
    const vec2f offset[4] = {vec2f(0, 1), vec2f(1), vec2f(1, 0), vec2f(0)};
    for(int i = 0, n; i < 4; i++)
    {
        if((n = i+1) == 4) n = 0;
        line(pos+size*offset[i], pos+size*offset[n], color);
    }
}

void curve(uint size, const vec2f *data, const rgba &color, float quality)
{
    if(size < 2)
        return;

    if(size == 2)
    {
        line(data[0], data[1], color);
        return;
    }

    if(quality <= 0)
        return;

    float length = 0;

    for(int i = 0; i < size; i++)
        length += data[i].x*data[i].x + data[i].y*data[i].y;

    length = sqrt(length);

    size--;

    vec2f temp = data[0];

    double add = quality/length;
    for(double t = add; t <= 1.0; t+=add)
    {
        vec2f current = {0, 0};
        for(int i = size; i >= 0; i--)
        {
            double mul = pow(1.0-t, size-i)*pow(t, i);
            if(i < size
            && i > 0)
                mul *= size;

            current += data[i] * mul;
        }

        line(temp, current, color);
        temp = current;
    }
}

void ark(const vec2f &pos, float radius, float angle, float value, const rgba &color, uint quality)
{
    if(!quality)
        quality = radius*3.1415*value/180.0f;

    vec2f temp = vec2f(sin(angle*DTOR)*radius+pos.x, cos(angle*DTOR)*radius+pos.y);
    for(int i = 1; i <= quality; i++)
    {
        vec2f result;
        sincosf((i*value/quality+angle)*DTOR, &result.x, &result.y);
        result *= radius;
        result += pos;
        line(temp, result, color);
        temp = result;
    }
}

void arkSolid(const vec2f &pos, float radius, float angle, float value, const rgba &color, uint quality)
{
    if(!quality)
        quality = radius*3.1415*value/180.0f;
    fillColor = color;
    vec2f temp = vec2f(sin(angle*DTOR)*radius+pos.x, cos(angle*DTOR)*radius+pos.y);
    for(int i = 1; i <= quality; i++)
    {
        vec2f result;
        sincosf((i*value/quality+angle)*DTOR, &result.x, &result.y);
        result *= radius;
        result += pos;
        triangle(temp, pos, result);
        temp = result;
    }
}

void arkBorder(const vec2f &pos, float radius, float angle, float value, const rgba &color, uint quality)
{
    if(!quality)
        quality = radius*3.1415*value/180.0f;

    vec2f temp = vec2f(sin(angle*DTOR)*radius+pos.x, cos(angle*DTOR)*radius+pos.y);
    line(temp, pos, color);
    for(int i = 1; i <= quality; i++)
    {
        vec2f result;
        sincosf((i*value/quality+angle)*DTOR, &result.x, &result.y);
        result *= radius;
        result += pos;
        line(temp, result, color);
        temp = result;
    }
    line(temp, pos, color);
}

void ring(const vec2f &pos, float radius, float radius2, const rgba &color, float angle, uint quality)
{
    if(radius < radius2)
        std::swap(radius, radius2);
    if(!quality)
        quality = radius*3.1415;

    fillColor = color;
    vec2f temp, result;
    sincosf(angle*DTOR, &temp.x, &temp.y);
    for(int i = 1; i <= quality; i++)
    {
        sincosf((i*360.0f/quality+angle)*DTOR, &result.x, &result.y);
        vec2f p1, p2, p3, p4;
        p1 = temp*radius+pos;
        p2 = temp*radius2+pos;
        p3 = result*radius+pos;
        p4 = result*radius2+pos;
        triangle(p1, p2, p3);
        triangle(p2, p3, p4);
        temp = result;
    }
}

void ringBorder(const vec2f &pos, float radius, float radius2, const rgba &color, float angle, uint quality)
{
    round(pos, radius, color, quality);
    round(pos, radius2, color, quality);
}

void ringArk(const vec2f &pos, float radius, float radius2, float angle, float value, const rgba &color, uint quality)
{
    if(radius < radius2)
        std::swap(radius, radius2);
    if(!quality)
        quality = radius*value/180.0f;
    fillColor = color;
    vec2f temp, result;
    sincosf(angle*DTOR, &temp.x, &temp.y);

    for(int i = 1; i <= quality; i++)
    {
        sincosf((i*value/quality+angle)*DTOR, &result.x, &result.y);
        vec2f p1, p2, p3, p4;
        p1 = temp*radius+pos;
        p2 = temp*radius2+pos;
        p3 = result*radius+pos;
        p4 = result*radius2+pos;
        triangle(p1, p2, p3);
        triangle(p2, p3, p4);
        temp = result;
    }
}

void ringArkBorder(const vec2f &pos, float radius, float radius2, float angle, float value, const rgba &color, uint quality)
{
    vec2f temp;
    sincosf(angle*DTOR, &temp.x, &temp.y);
    line(temp*radius+pos, temp*radius2+pos, color);
    sincosf((angle+value)*DTOR, &temp.x, &temp.y);
    line(temp*radius+pos, temp*radius2+pos, color);
    ark(pos, radius, angle, value, color, quality);
    ark(pos, radius2, angle, value, color, quality);
}

void drawGui(const vec2f &pos, float type, const vec2f &size, const vec4f &color)
{
    const float borderSize = 4.0;
    const vec2f offset[6] = {{0, 1}, {1, 1}, {1, 0},
                       {1, 0}, {0, 0}, {0, 1}};
    if(size.x == 0)
        return;

    const vec2f texCoord[9][6] =
        {{vec2f(0, 0), vec2f(1, 0), vec2f(1, 1), vec2f(1, 1), vec2f(0, 1), vec2f(0, 0)},
         {vec2f(1, 0), vec2f(1, 0), vec2f(1, 1), vec2f(1, 1), vec2f(1, 1), vec2f(1, 0)},
         {vec2f(1, 0), vec2f(0, 0), vec2f(0, 1), vec2f(0, 1), vec2f(1, 1), vec2f(1, 0)},

         {vec2f(0, 0), vec2f(1, 0), vec2f(1, 0), vec2f(1, 0), vec2f(0, 0), vec2f(0, 0)},
         {vec2f(1, 0), vec2f(1, 0), vec2f(1, 0), vec2f(1, 0), vec2f(1, 0), vec2f(1, 0)},
         {vec2f(1, 0), vec2f(0, 0), vec2f(0, 0), vec2f(0, 0), vec2f(1, 0), vec2f(1, 0)},

         {vec2f(0, 1), vec2f(1, 1), vec2f(1, 0), vec2f(1, 0), vec2f(0, 0), vec2f(0, 1)},
         {vec2f(1, 1), vec2f(1, 1), vec2f(1, 0), vec2f(1, 0), vec2f(1, 0), vec2f(1, 1)},
         {vec2f(1, 1), vec2f(0, 1), vec2f(0, 0), vec2f(0, 0), vec2f(1, 0), vec2f(1, 1)}};

    vec2f posList[9] = {vec2f(-borderSize, -borderSize), vec2f(0, -borderSize), vec2f(size.x, -borderSize),
                        vec2f(-borderSize, 0), vec2f(0, 0), vec2f(size.x, 0),
                        vec2f(-borderSize, size.y), vec2f(0, size.y), vec2f(size.x, size.y)};

    vec2f sizeList[9] = {vec2f(borderSize, borderSize), vec2f(size.x, borderSize), vec2f(borderSize, borderSize),
                         vec2f(borderSize, size.y), vec2f(size.x, size.y), vec2f(borderSize, size.y),
                         vec2f(borderSize, borderSize), vec2f(size.x, borderSize), vec2f(borderSize, borderSize)};

    type3D::texVert data[9*6];
    uint addr = 0;
    for(int c = 0; c < 9; c++)
        for(int i = 0; i < 6; i++)
        {
            data[addr].vert = vec3f(offset[i]*sizeList[c]+posList[c]+pos, type);
            data[addr].texCoord = texCoord[c][i];
            data[addr++].color = color;
        }
    render::defaultResources::guiShader.bind();
    render::defaultResources::guiTextures[0].bind(0);
    render::defaultResources::guiTextures[1].bind(1);
    render::_internal::_addTriangles3D(data, addr);
}

END_PRIMITIVES2D_NAMESPACE
