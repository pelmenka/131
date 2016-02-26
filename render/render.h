#pragma once

namespace render
{
    class font;
}

#include <list>

#include "../common.h"
#include "primitives.h"
#include "type.h"

#include "shader.h"
#include "texture.h"
#include "buffer.h"


namespace render
{
    enum class blendMode
    {
        normal = 0,
        add
    };

    void initRender();
    void closeRender();

    void setClearColor(const vec4f& = vec4f(0, 0, 0, 1));
    void clearScreen();
    void endDraw();

    void mode2D(vec2i size = vec2f(-1), const vec2i &offset = vec2f());
    void mode3D(float fov = 60, vec2i size = vec2f(-1));
    void viewport(const vec2u &size, const vec2i &offset = vec2f());
    void setBlendMode(blendMode);

    const texture *getActiveTexture(uint);
    const shader *getActiveShader();

    namespace defaultResources
    {
        extern shader regularShader, primitivesShader, particlesShader, textShader, guiShader;
        extern texture guiTextures[2];
        extern font textFont;
    }

    namespace _internal
    {
        void _bindTexture(uint, const texture*);
        void _bindShader(const shader*);
        inline int _getMaxBindedTextures();
        int _getTextureSlot(const texture*);
        bool _isTextureBinded(const texture*, uint);

        void _addTriangles2D(const type2D::texVert*, uint);
        void _addLines2D(const type2D::vertex*, uint);
        void _addPoints2D(const type2D::vertex*, uint);
        void _addParticles2D(const type2D::particle*, uint);

        void _addTriangles3D(const type3D::texVert*, uint);

        extern std::list<render::_object*> objectList;
    }
}

#define CHECK_DEF_SHAD(x) \
        {\
            const shader *__DEF_SHAD = render::getActiveShader();\
            if(!__DEF_SHAD\
            || __DEF_SHAD->type != shader::shaderType::x\
            || __DEF_SHAD->standart)\
                render::defaultResources::x##Shader.bind();\
        }

