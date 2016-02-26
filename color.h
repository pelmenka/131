#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include "common.h"

vec4f mathColor(vec4f, vec4f);
vec4f Hex2RGBA(uint, float alpha = 1.0);
vec3f Hex2RGB(uint);
vec4f hsv2RGBA(vec3f, float alpha = 1.0);
vec4f hsv2RGBA(vec4f);
vec3f hsv2RGB(vec3f);
vec3f hsv2RGB2(vec3f);

uint RGB2Hex(vec3f);

vec3f RGB2HSV(vec3f);
vec3f RGBA2HSV(vec4f);

#endif // COLOR_H_INCLUDED
