#include <GL/glew.h>
#include <FreeImage.h>
#include <memory>
#include "texture.h"
#include "render.h"
#include "../log.h"

namespace render{

texture::texture(){}

texture::texture(const std::string &path, bool smooth, bool mipmap):texture()
{
    load(path, smooth, mipmap);
}

texture::~texture()
{
    free();
}

bool texture::load(const std::string &path, bool smooth, bool mipmap)
{
    _name = path;
    for(auto &i: render::_internal::objectList)
        if(texture *tex = dynamic_cast<texture*>(i))
            if(tex->_name == _name)
            {
                _log::out("Found texture: %s [ OK ]\n", path.c_str());
                _obj = tex->_obj;
                _size = tex->_size;
                return 1;
            }

	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path.c_str(), 0);
	FIBITMAP* image = FreeImage_Load(format, path.c_str());

	_log::out("Loading texture: %s ", path.c_str());

	if(!image)
    {
        _log::out("[FAIL]\n");
        return 0;
    }
	image = FreeImage_ConvertTo32Bits(image);
	_size.x = FreeImage_GetWidth(image);
	_size.y = FreeImage_GetHeight(image);
	uchar* pixeles = FreeImage_GetBits(image);
    _alloc();
    bind();
	if(mipmap) gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, _size.x, _size.y, GL_BGRA, GL_UNSIGNED_BYTE, pixeles);
    else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _size.x, _size.y, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixeles);
    if(smooth)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
	FreeImage_Unload(image);
	_log::out("[ OK ] (%dx%d)\n", _size.x, _size.y);
	return 1;
}

void texture::create(const vec2u &size, bool smooth)
{
    _alloc();
    bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	if(smooth)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    _size = size;
}

void texture::copyScreen(const vec2i& position, vec2u size)
{
    bind();
    if(!size.x) size.x = _size.x;
    if(!size.y) size.y = _size.y;
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, position.x, position.y, size.x, size.y, 0);
    _size = size;
}

void texture::_alloc()
{
    if(_obj.use_count())
        return;
    _obj = std::make_shared<__glTexture>(_name);
    _addToList();
}

void texture::free()
{
    if(!_obj.use_count())
        return;
    unbind();
    //glDeleteTextures(1, &obj);
    _obj = nullptr;
    printf("free %s\n", _name.c_str());
    _removeFromList();
}

void texture::bind(uint s) const
{
    if(!_obj)
        return;
    static uint slot = 0;
    if(render::getActiveTexture(s) == this)
        return;
    render::_internal::_bindTexture(s, this);
    if(slot != s)
    {
        glActiveTexture(GL_TEXTURE0+s);
        slot = s;
    }
    glBindTexture(GL_TEXTURE_2D, _obj->obj);
}

void texture::unbind(int s)
{
    if(s < 0)
        while((s = render::_internal::_getTextureSlot(this)) != -1)
        {
            render::_internal::_bindTexture(s, 0);
            glActiveTexture(GL_TEXTURE0+s);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    else
    {
        render::_internal::_bindTexture(s, 0);
        glActiveTexture(GL_TEXTURE0+s);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void texture::parameteri(uint param, uint value)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, param, value);
}
}

__glTexture::__glTexture(const std::string &name):_name(name)
{
    glGenTextures(1, &obj);
}

__glTexture::~__glTexture()
{
    glDeleteTextures(1, &obj);

}
