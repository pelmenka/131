#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "render.h"

#include "shader.h"
#include "../main.h"
#include "../color.h"
#include "../window.h"
#include "font.h"

#include "texture.h"
#include "buffer.h"
#include "image.h"
#include "atlas.h"

int maxBindedTextures;
struct _state
{
    bool renderMode;
    render::blendMode blendMode;
    uint drawType;
    const render::texture **bindedTextures;
    const render::shader *bindedShader;
}_oldState;

uint _count;
render::type2D::vertex *_vert2D;
render::type2D::texVert *_tex2D;
render::type2D::particle *_part2D;

render::type3D::vertex *_vert3D;
render::type3D::texVert *_tex3D;
render::type3D::particle *_part3D;

void draw2DPoints(const render::type2D::vertex* buffer, uint size);
void draw2DLines(const render::type2D::vertex* buffer, uint size);
void draw2DTriangles(const render::type2D::texVert* buffer, uint size);
void draw2DParticles(const render::type2D::particle* buffer, uint size);

//void draw3DPrimitives(const void* buffer, uint size);
void draw3DTriangles(const render::type3D::texVert* buffer, uint size);
//void draw3DParticles(const void* buffer, uint size);

std::list<render::_object*> render::_internal::objectList;

struct
{
    render::vertexArray VAO[3];
    render::vertexBuffer VBO[3];
}buffers2D, buffers3D;

void loadResources();
void initBuffers();

template<typename T>
inline void _drawElements(T*, uint, uint, render::vertexArray*, render::vertexBuffer*);

const size_t meshBufferSize = 0xfff;

namespace render
{
    void setClearColor(const vec4f &color)
    {
        glClearColor(color.x, color.y, color.z, color.w);
    }

    void clearScreen()
    {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }

    void initRender()
    {
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxBindedTextures);
        _oldState.bindedTextures = new const texture*[maxBindedTextures];

        _oldState.renderMode = 1;
        _oldState.blendMode = blendMode::normal;

        glEnable(GL_TEXTURE_2D);
        glCullFace(GL_BACK);

        glDepthFunc(GL_LEQUAL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);
        glEnable(GL_LINE_SMOOTH);

        loadResources();
        initBuffers();
        _count = 0;

        mode2D();
    }

    void closeRender()
    {
        while(_internal::objectList.size())
            _internal::objectList.back()->free();
        delete [] _vert2D;
        delete [] _tex2D;
        delete [] _part2D;
        delete [] _tex3D;
        delete [] _oldState.bindedTextures;
    }

    void viewport(const vec2u &size, const vec2i &offset)
    {
        glViewport(offset.x, offset.y, size.x, size.y);
    }

    void mode2D(vec2i size, const vec2i &offset)
    {
        if(_oldState.renderMode)
        {
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            _oldState.renderMode = 0;
        }
        if(size.x == -1) size.x = window::getSize().x;
        if(size.y == -1) size.y = window::getSize().y;
        glLoadIdentity();
        gluOrtho2D(offset.x, size.x, size.y, offset.y);
    }

    void mode3D(float fov, vec2i size)
    {
        if(!_oldState.renderMode)
        {
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            _oldState.renderMode = 1;
        }
        glLoadIdentity();
        if(size.x == -1) size.x = window::getSize().x;
        if(size.y == -1) size.y = window::getSize().y;
        gluPerspective(fov, float(size.x)/size.y, 0.1, 100);
    }


    const shader *getActiveShader()
    {
        return _oldState.bindedShader;
    }

    const texture *getActiveTexture(uint id)
    {
        if(maxBindedTextures < id)
            return nullptr;
        return _oldState.bindedTextures[id];
    }

    void endDraw()
    {
        if(!_count)
            return;
        switch(_oldState.drawType)
        {
        case 0: //points
            _drawElements(_vert2D, _count, GL_POINTS, &buffers2D.VAO[0], &buffers2D.VBO[0]); break;
        case 1:
            _drawElements(_vert2D, _count, GL_LINES, &buffers2D.VAO[0], &buffers2D.VBO[0]); break;
        case 2:
            _drawElements(_tex2D, _count, GL_TRIANGLES, &buffers2D.VAO[1], &buffers2D.VBO[1]); break;
        case 3:
            _drawElements(_part2D, _count, GL_POINTS, &buffers2D.VAO[2], &buffers2D.VBO[2]); break;
        /*case 4: //poins
            break;
        case 5: //lines
            break;*/
        case 6: //triangles
            _drawElements(_tex3D, _count, GL_TRIANGLES, &buffers3D.VAO[1], &buffers3D.VBO[1]); break;
        }
        _count = 0;
    }

    namespace _internal{

    void _bindShader(const shader *shad)
    {
        if(_oldState.bindedShader != shad)
            endDraw();
        _oldState.bindedShader = shad;
    }

    void  _bindTexture(uint id, const texture *tex)
    {
        if(maxBindedTextures < id)
            return;
        if(_oldState.bindedTextures[id] != tex)
            endDraw();
        _oldState.bindedTextures[id] = tex;
    }

    int _getTextureSlot(const texture *tex)
    {
        for(int i = 0; i < maxBindedTextures; i++)
            if(_oldState.bindedTextures[i] == tex)
                return i;
        return -1;
    }

    bool _isTextureBinded(const texture *tex, uint id)
    {
        if(id >= maxBindedTextures) return 0;
        return _oldState.bindedTextures[id] == tex;
    }

    void _addPoints2D(const type2D::vertex *data, uint count)
    {
        if(_oldState.drawType != 0 || _count + count > meshBufferSize)
            endDraw();
        _oldState.drawType = 0;
        memcpy(&_vert2D[_count], data, count*sizeof(type2D::vertex));
        _count += count;
    }

    void _addLines2D(const type2D::vertex *data, uint count)
    {
        if(_oldState.drawType != 1 || _count + count > meshBufferSize)
            endDraw();
        _oldState.drawType = 1;
        memcpy(&_vert2D[_count], data, count*sizeof(type2D::vertex));
        _count += count;
    }

    void _addTriangles2D(const type2D::texVert *data, uint count)
    {
        if(_oldState.drawType != 2 || _count + count > meshBufferSize)
            endDraw();
        _oldState.drawType = 2;
        memcpy(&_tex2D[_count], data, count*sizeof(type2D::texVert));
        _count += count;
    }

    void _addParticles2D(const type2D::particle *data, uint count)
    {
        if(_oldState.drawType != 3 || _count + count > meshBufferSize)
            endDraw();
        _oldState.drawType = 3;
        uint addr = 0;
        while(count)
        {
            if(count >= meshBufferSize)
            {
                _count = meshBufferSize;
                memcpy(_part2D, &data[addr], meshBufferSize*sizeof(type2D::particle));
                endDraw();
                count -= meshBufferSize;
                addr += meshBufferSize;
            }else
            {
                _count = count;
                memcpy(_part2D, &data[addr], count*sizeof(type2D::particle));
                endDraw();
                break;
            }
        }
    }

    void _addTriangles3D(const type3D::texVert *data, uint count)
    {
        if(_oldState.drawType != 6 || _count + count > meshBufferSize)
            endDraw();
        _oldState.drawType = 6;
        memcpy(&_tex3D[_count], data, count*sizeof(type3D::texVert));
        _count += count;
    }
    }

    namespace defaultResources
    {
        shader regularShader, primitivesShader, particlesShader, textShader, guiShader;
        texture guiTextures[2];
        font textFont;
    }
}

void loadResources()
{
    render::defaultResources::particlesShader.load("data/shaders/particles.vert", "data/shaders/particles.frag", render::shader::shaderType::particles, 1);
    render::defaultResources::primitivesShader.load("data/shaders/primitives.vert", "data/shaders/primitives.frag", render::shader::shaderType::primitives, 1);
    render::defaultResources::regularShader.load("data/shaders/regular.vert", "data/shaders/regular.frag", render::shader::shaderType::regular, 1);
    render::defaultResources::textShader.load("data/shaders/text.vert", "data/shaders/text.frag", render::shader::shaderType::text, 1);
    render::defaultResources::guiShader.load("data/shaders/gui.vert", "data/shaders/gui.frag", render::shader::shaderType::gui, 1);
    render::defaultResources::guiShader.uniform1i("tex2", 1);


    render::defaultResources::guiTextures[0].load("data/textures/buttonShape.png", 1);
    render::defaultResources::guiTextures[1].load("data/textures/buttonColor.png", 1);
    render::defaultResources::textFont.load("data/font");

    render::defaultResources::guiTextures[0].parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    render::defaultResources::guiTextures[0].parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    render::defaultResources::guiTextures[1].parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    render::defaultResources::guiTextures[1].parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

///=============================================================

template<typename T>
inline void _drawElements(T *data, uint count, uint mode, render::vertexArray *VAO, render::vertexBuffer *VBO)
{
    VBO->subData(count, data);
    VAO->draw(mode, 0, count);
}

void initBuffers()
{
    using namespace render;
    _vert2D = new type2D::vertex[meshBufferSize];
    _tex2D = new type2D::texVert[meshBufferSize];
    _part2D = new type2D::particle[meshBufferSize];
    //_vert3D = new type3D::texVert[mechBufferSize];
    _tex3D = new type3D::texVert[meshBufferSize];

    for(int i = 0; i < 3; i++)
    {
        buffers2D.VAO[i].gen();
        buffers2D.VBO[i].gen();
        buffers3D.VAO[i].gen();
        buffers3D.VBO[i].gen();
    }

    buffers2D.VBO[0].createEmptyBuffer(meshBufferSize*sizeof(type2D::vertex));
    buffers2D.VBO[1].createEmptyBuffer(meshBufferSize*sizeof(type2D::texVert));
    buffers2D.VBO[2].createEmptyBuffer(meshBufferSize*sizeof(type2D::particle));

    //buffers3D.VBO[0].createEmptyBuffer(meshBufferSize*sizeof(type3D::vertex));
    buffers3D.VBO[1].createEmptyBuffer(meshBufferSize*sizeof(type3D::texVert));
    //buffers3D.VBO[2].createEmptyBuffer(meshBufferSize*sizeof(type3D::particle));

    //(* 2D

    { //для точек и линий
        buffers2D.VAO[0].bind();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        buffers2D.VBO[0].bind();

        glVertexPointer(3, GL_FLOAT, sizeof(type2D::vertex), 0);
        glColorPointer(4, GL_FLOAT, sizeof(type2D::vertex), (GLuint*)(sizeof(type2D::vertex::vert)));
    }

    { //для треугольников
        buffers2D.VAO[1].bind();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        buffers2D.VBO[1].bind();

        glVertexPointer(2, GL_FLOAT, sizeof(type2D::texVert), 0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(type2D::texVert), (GLuint*)(sizeof(type2D::texVert::vert)));
        glColorPointer(4, GL_FLOAT, sizeof(type2D::texVert), (GLuint*)( sizeof(type2D::texVert::vert) + sizeof(type2D::texVert::texCoord)));
    }

    { //для частиц
        buffers2D.VAO[2].bind();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        buffers2D.VBO[2].bind();
        glVertexPointer(3, GL_FLOAT, sizeof(type2D::particle), 0);
        glTexCoordPointer(4, GL_FLOAT, sizeof(type2D::particle), (GLuint*)(sizeof(vec3f)));
        glColorPointer(4, GL_FLOAT, sizeof(type2D::particle), (GLuint*)(sizeof(vec3f)+sizeof(vec4f)));
    }

    //*)
    //(* 3D
    {
        buffers3D.VAO[1].bind();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        buffers3D.VBO[1].bind();

        glVertexPointer(3, GL_FLOAT, sizeof(type3D::texVert), 0);
        glTexCoordPointer(2, GL_FLOAT, sizeof(type3D::texVert), (GLuint*)(sizeof(type3D::texVert::vert)));
        glColorPointer(4, GL_FLOAT, sizeof(type3D::texVert), (GLuint*)( sizeof(type3D::texVert::vert) + sizeof(type3D::texVert::texCoord)));
    }

    /*{
        buffers3D.VAO[2].bind();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        buffers3D.VBO[2].bind();

        glVertexPointer(4, GL_FLOAT, sizeof(type3D::particle), 0);
        glTexCoordPointer(4, GL_FLOAT, sizeof(type3D::particle), (GLuint*)(sizeof(vec4f)));
        glColorPointer(4, GL_FLOAT, sizeof(type3D::particle), (GLuint*)( sizeof(vec4f) + sizeof(vec4f)));
    }*/
}

///===================================

namespace render{

void _object::_removeFromList()
{
    render::_internal::objectList.remove(this);
}

void _object::_addToList()
{
    render::_internal::objectList.push_back(this);
}

}
