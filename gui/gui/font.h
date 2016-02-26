#ifndef _GUI_FONT_H_INCLUDED
#define _GUI_FONT_H_INCLUDED

#define DATA_BUFFER_SIZE 0xffc //кратно 6, 2 треугольника на символ

#include "../../render/render.h"
#include "../../render/texture.h"
#include "../../common.h"
#include <vector>
#include <string>

namespace gui{

class font
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
    font(std::string path);
    //~font();

    bool load(std::string path);
    void draw(const drawGlyphInfo*, uint, float scale);

    void flush(float);

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

    render::type2D::texVert *_data;
    uint _drawCount;

    render::texture _tex;
    std::vector<_charInfo> _charList;
    //std::vector<_keriningInfo> _keriningList;
};

}

#endif // _GUI_FONT_H_INCLUDED
