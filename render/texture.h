#pragma once

#include <string>
#include <memory>
#include "../common.h"
#include "object.h"

#define SMOOTH_BY_DEFAULT 0

class __glTexture
{
public:
    const std::string &_name;
    __glTexture(const std::string&);
    ~__glTexture();
    uint obj;
};

namespace render{

class texture: public _object
{
public:
    texture();
    texture(const std::string &path, bool filter = SMOOTH_BY_DEFAULT, bool mipmap = 0);
    ~texture();

    virtual bool load(const std::string &path, bool filter = SMOOTH_BY_DEFAULT, bool mipmap = 0);
    void create(const vec2u& size, bool filter = SMOOTH_BY_DEFAULT);
    void copyScreen(const vec2i& position, vec2u size = vec2u());

    void free();

    void parameteri(uint, uint);

    void bind(uint slot = 0) const;
    void unbind(int slot = -1);

    inline const vec2u &getSize() const noexcept;

    uint getGLTexture();

    inline const std::string &name() const noexcept;
protected:
    std::string _name;
    void _alloc();
    vec2u _size;
    //uint obj;
    std::shared_ptr<__glTexture> _obj;
};
}
