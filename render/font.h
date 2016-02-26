#ifndef _RENDER_FONT_H_INCLUDED
#define _RENDER_FONT_H_INCLUDED

#define DATA_BUFFER_SIZE 0xffc //кратно 6, 2 треугольника на символ

#include "render.h"
#include "object.h"
#include "texture.h"
#include "../common.h"
#include <vector>
#include <string>

namespace render{

class font:texture
{
    friend class text;
    struct drawGlyphInfo
    {
        uint code;
        vec2f pos;
        vec4f color;
    };
public:
    font();
    font(const std::string &path);
    //~font();

    bool load(const std::string &path);
    void draw(const drawGlyphInfo*, uint, float scale) const;

protected:

    struct _kerningInfo
    {
        uint code;
        float offset;
    };

    struct _charInfo
    {
        uint code;
        vec4f texCoord;
        vec2i size;
        vec2i imgSize;
        vec2i offset;
        std::vector<_kerningInfo> kerning;
    };

    float defaultScale;
    std::vector<_charInfo> _charList;
    //std::vector<_keriningInfo> _keriningList;
};

}

#endif // _RENDER_FONT_H_INCLUDED
