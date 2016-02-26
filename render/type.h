#ifndef _RENDER_TYPE_H_INCLUDED
#define _RENDER_TYPE_H_INCLUDED

#include "../common.h"

typedef vec4f rgba;

namespace render
{
    namespace type2D
    {
        struct texVert
        {
            vec2f vert;
            vec2f texCoord;
            rgba color;
        };

        struct vertex
        {
            vec3f vert;
            rgba color;
        };

        struct particle
        {
            vec3f vert;
            vec4f tex;
            vec3f color;
            float alpha;
        };
    }

    namespace type3D
    {
        struct texVert
        {
            vec3f vert;
            vec2f texCoord;
            rgba color;
        };

        struct vertex
        {
            vec4f vert;
            rgba color;
        };

        struct particle
        {
            vec4f vert;
            vec4f tex;
            vec3f color;
            float alpha;
        };
    }
}

#endif // _RENDER_TYPE_H_INCLUDED
