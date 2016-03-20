#pragma once

#include "render.h"
#include "type.h"

typedef vec4f rgba;

namespace render{
namespace primitives2d{

extern rgba pointColor, lineColor, fillColor;

// �����
void point(const vec2f&, const rgba& = pointColor, float width = 1.0);

// �����
void line(const vec2f&, const vec2f&, const rgba& = lineColor);
inline void line(const vec2f&, const vec2f&, const rgba&, const rgba&);

// �����������
void triangle(const vec2f&, const vec2f&, const vec2f&, // x/y
              const vec2f& = {0, 0}, const vec2f& = {0, 0}, const vec2f& = {0, 0}, // u/v
              const rgba& = fillColor, const rgba& = fillColor, const rgba& = fillColor); // color

//�������
void quad(const vec2f& pos, const vec2f& size, const rgba& = fillColor);

//���� �� �������������
void circle(const vec2f& pos, float radius, const rgba& = fillColor, float angle = 0, uint edges = 0);

//���������� �� �����
void round(const vec2f& pos, float radius, const rgba& = fillColor, float angle = 0, uint edges = 0);

//������� �� �����
void rect(const vec2f&, const vec2f&, const rgba& = lineColor);

//������
void curve(uint size, const vec2f *data, const rgba& = lineColor, float quality = 1);

//����
void ark(const vec2f& pos, float radius, float angle, float value, const rgba& = lineColor, uint quality = 0);
void arkSolid(const vec2f& pos, float radius, float angle, float value, const rgba& = fillColor, uint quality = 0);
void arkBorder(const vec2f& pos, float radius, float angle, float value, const rgba& = lineColor, uint quality = 0);

//������
void ring(const vec2f &pos, float radius, float radius2, const rgba& = fillColor, float angle = 0, uint quality = 0);
void ringBorder(const vec2f &pos, float radius, float radius2, const rgba& = fillColor, float angle = 0, uint quality = 0);

//������-���� �_�
void ringArk(const vec2f &pos, float radius, float radius2, float angle, float value, const rgba& = fillColor, uint quality = 0);
void ringArkBorder(const vec2f &pos, float radius, float radius2, float angle, float value, const rgba& = lineColor, uint quality = 0);

void drawGui(const vec2f& pos, float type, const vec2f& size, const rgba& color);

}

}
