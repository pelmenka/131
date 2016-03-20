#pragma once

#include "render.h"
#include "type.h"

typedef vec4f rgba;

namespace render{
namespace primitives2d{

extern rgba pointColor, lineColor, fillColor;

// точка
void point(const vec2f&, const rgba& = pointColor, float width = 1.0);

// линия
void line(const vec2f&, const vec2f&, const rgba& = lineColor);
inline void line(const vec2f&, const vec2f&, const rgba&, const rgba&);

// треугольник
void triangle(const vec2f&, const vec2f&, const vec2f&, // x/y
              const vec2f& = {0, 0}, const vec2f& = {0, 0}, const vec2f& = {0, 0}, // u/v
              const rgba& = fillColor, const rgba& = fillColor, const rgba& = fillColor); // color

//квадрат
void quad(const vec2f& pos, const vec2f& size, const rgba& = fillColor);

//круг из треугольников
void circle(const vec2f& pos, float radius, const rgba& = fillColor, float angle = 0, uint edges = 0);

//окружность из линий
void round(const vec2f& pos, float radius, const rgba& = fillColor, float angle = 0, uint edges = 0);

//квадрат из линий
void rect(const vec2f&, const vec2f&, const rgba& = lineColor);

//кривая
void curve(uint size, const vec2f *data, const rgba& = lineColor, float quality = 1);

//дуги
void ark(const vec2f& pos, float radius, float angle, float value, const rgba& = lineColor, uint quality = 0);
void arkSolid(const vec2f& pos, float radius, float angle, float value, const rgba& = fillColor, uint quality = 0);
void arkBorder(const vec2f& pos, float radius, float angle, float value, const rgba& = lineColor, uint quality = 0);

//кольцо
void ring(const vec2f &pos, float radius, float radius2, const rgba& = fillColor, float angle = 0, uint quality = 0);
void ringBorder(const vec2f &pos, float radius, float radius2, const rgba& = fillColor, float angle = 0, uint quality = 0);

//кольцо-дуга о_о
void ringArk(const vec2f &pos, float radius, float radius2, float angle, float value, const rgba& = fillColor, uint quality = 0);
void ringArkBorder(const vec2f &pos, float radius, float radius2, float angle, float value, const rgba& = lineColor, uint quality = 0);

void drawGui(const vec2f& pos, float type, const vec2f& size, const rgba& color);

}

}
