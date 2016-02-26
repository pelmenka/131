#pragma once
#include "texture.h"

namespace render{
class image:public texture
{
public:
    vec2d center;
    vec2f scale;
    vec4f color;
    vec2u parts;
    float angle;

    image();
    image(const std::string &path, bool filter = SMOOTH_BY_DEFAULT, bool mipmap = 0);
    ~image();

    bool load(const std::string &path, bool filter = SMOOTH_BY_DEFAULT, bool mipmap = 0);
    void create(const vec2u& size, bool filter = SMOOTH_BY_DEFAULT);
    void copyScreen(const vec2i& position, const vec2u& size);

    void draw(const vec2f &pos = vec2f()) noexcept;
    void draw(const vec2f &pos, const vec2u &part) noexcept;
private:
    vec2f _rotMatr[6];
    void _mathRotate();
    inline void _checkParams();

    vec2u _oldparts;
    vec2f _oldscale;
    vec2f _oldcenter;
    float _oldangle;
};
}
