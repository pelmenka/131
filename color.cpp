#include "color.h"
#include <cmath>

vec4f mathColor(vec4f r, vec4f c)
{
    if(c.x >= 0) r.x = c.x;
    if(c.y >= 0) r.y = c.y;
    if(c.z >= 0) r.z = c.z;
    if(c.w >= 0) r.w = c.w;
    return r;
}

vec3f Hex2RGB(uint color)
{
    vec3f result;
    result.x = (float)((color>>16)&0xff)/255.0;
    result.y = (float)((color>>8)&0xff)/255.0;
    result.z = (float)((color)&0xff)/255.0;
    return result;
}

uint RGB2Hex(vec3f color)
{
    uint res = 0;
    res |= ((uint)(color.x*255)&0xff)<<16;
    res |= ((uint)(color.y*255)&0xff)<<8;
    res |= ((uint)(color.z*255)&0xff);
    return res;
}

vec4f Hex2RGBA(uint color, float alpha)
{
    vec4f result;
    result.w = alpha;

    result.x = (float)((color>>16)&0xff)/256.0;
    result.y = (float)((color>>8)&0xff)/256.0;
    result.z = (float)((color)&0xff)/256.0;
    return result;
}

vec3f hsv2RGB(vec3f in)
{
    in.x = fmod(in.x, 360);

    float vmin = in.z*(1.0-in.y);
    float a = (in.z - vmin)*((int)in.x%60)/60;
    float vinc = vmin+a;
    float vdec = in.z-a;

    switch(int(in.x/60))
    {
    case 0:  return vec3f(in.z, vinc, vmin);
    case 1:  return vec3f(vdec, in.z, vmin);
    case 2:  return vec3f(vmin, in.z, vinc);
    case 3:  return vec3f(vmin, vdec, in.z);
    case 4:  return vec3f(vinc, vmin, in.z);
    default: return vec3f(in.z, vmin, vdec);
    }
}

vec4f hsv2RGBA(vec3f c, float a)
{
    vec3f temp = hsv2RGB(c);
    return {temp.x, temp.y, temp.z, a};
}

vec4f hsv2RGBA(vec4f c)
{
    vec3f temp = hsv2RGB(vec3f(c.x, c.y, c.z));
    return {temp.x, temp.y, temp.z, c.w};
}

vec3f RGB2HSV(vec3f in)
{
    vec3f result;
    float cmax = max(max(in.x, in.y), in.z);
    float cmin = min(min(in.x, in.y), in.z);
    float delta = cmax-cmin;

    if(delta == 0)
        result.x = 0;
    else if(cmax == in.x)
        result.x = fmod((in.y-in.z)/delta, 6);
    else if(cmax == in.y)
        result.x = (in.z-in.x)/delta+2;
    else if(cmax == in.z)
        result.x = (in.x-in.y)/delta+4;
    result.x*=60;
    if(cmax == 0)
        result.y = 0;
    else result.y = delta/cmax;
    result.z = cmax;
    return result;
}
