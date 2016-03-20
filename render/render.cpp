#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "render.h"
#include "shader.h"
#include "../main.h"
#include "../color.h"
#include "../window.h"
#include "../log.h"
#include "font.h"

#include "texture.h"
#include "buffer.h"
#include "image.h"
#include "atlas.h"

int maxBindedTextures;
bool binaryShadersSupported;
struct _state
{
    bool renderMode;
    render::blendMode blendMode;
    uint drawType;
    const render::texture **bindedTextures;
    const render::shader *bindedShader;
    vec2u windowSize;
    vec2i offset;
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

struct
{
    glm::mat4 projection, view;
}mat2D, mat3D;

namespace render
{

    _camInfo camera;
    void setClearColor(const vec4f &color)
    {
        glClearColor(color.x, color.y, color.z, color.w);
    }

    void setBlendMode(blendMode mode)
    {
        if(_oldState.blendMode == mode)
            return;

        switch(mode)
        {
        case blendMode::add:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        default:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        _oldState.blendMode = mode;
    }

    void clearScreen()
    {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }

    void initRender()
    {
        binaryShadersSupported = __GLEW_ARB_get_program_binary;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxBindedTextures);
        _oldState.bindedTextures = new const texture*[maxBindedTextures];

        _oldState.renderMode = 1;
        _oldState.blendMode = blendMode::normal;

        glCullFace(GL_BACK);

        glDepthFunc(GL_LEQUAL);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_LINE_SMOOTH);

        loadResources();
        initBuffers();
        _count = 0;

        mode2D();
    }

    void closeRender()
    {
        _log::out("\n============== clean up ================\n");
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
        mat2D.projection = glm::ortho((float)offset.x, (float)size.x, (float)size.y, (float)offset.y);
        mat2D.projection = glm::translate(mat2D.projection, glm::vec3(-camera.pos.x, -camera.pos.y, 0));
        defaultResources::primitivesShader.loadProjectionMatrix(mat2D.projection);
        defaultResources::regularShader.loadProjectionMatrix(mat2D.projection);
        defaultResources::particlesShader.loadProjectionMatrix(mat2D.projection);
        defaultResources::textShader.loadProjectionMatrix(mat2D.projection);
        defaultResources::guiShader.loadProjectionMatrix(mat2D.projection);
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
        if(size.x == -1) size.x = window::getSize().x;
        if(size.y == -1) size.y = window::getSize().y;
        glTranslatef(camera.pos.x, camera.pos.y, camera.pos.z);
        mat3D.projection = glm::perspective(60.0f, float(size.x)/size.y, 0.1f, 100.0f);
        defaultResources::primitivesShader.loadProjectionMatrix(mat3D.projection);
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
        uint addr = 0;
        while(count)
        {
            if(count >= meshBufferSize)
            {
                _count = meshBufferSize;
                memcpy(_tex2D, &data[addr], meshBufferSize*sizeof(type2D::texVert));
                endDraw();
                count -= meshBufferSize;
                addr += meshBufferSize;
            }else
            {
                _count += count;
                memcpy(_tex2D, &data[addr], count*sizeof(type2D::texVert));
                endDraw();
                break;
            }
        }
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
                _count += count;
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
        shader regular3DShader;
        texture guiTextures[2];
        font textFont;
    }
}

void loadResources()
{
    using namespace render;
    using namespace defaultResources;
    _log::out("\n=================start loading default resources================\n");
    //render::defaultResources::particlesShader.load("data/shaders/particles.vert", "data/shaders/particles.frag", render::shader::shaderType::particles, 1);


    if(!primitivesShader.loadBinary(SHADER_BINARY_PATH"primitives.shad"))
    {
        primitivesShader.attach(SHADER_SOURCE_PATH"primitives.vert", SHADER_VERTEX);
        primitivesShader.attach(SHADER_SOURCE_PATH"primitives.frag", SHADER_FRAGMENT);
        primitivesShader.bindAttribLocation("vertex", 0);
        primitivesShader.bindAttribLocation("color", 2);
        primitivesShader.linkProgram();
    }
    primitivesShader.validate();
    primitivesShader.type = shader::shaderType::primitives;
    primitivesShader.standart = 1;


    if(!regularShader.loadBinary(SHADER_BINARY_PATH"regular.shad"))
    {
        regularShader.attach(SHADER_SOURCE_PATH"regular.vert", SHADER_VERTEX);
        regularShader.attach(SHADER_SOURCE_PATH"regular.frag", SHADER_FRAGMENT);
        regularShader.bindAttribLocation("vertex", 0);
        regularShader.bindAttribLocation("texCoord", 1);
        regularShader.bindAttribLocation("color", 2);
        regularShader.linkProgram();
    }
    regularShader.validate();
    regularShader.type = shader::shaderType::regular;
    regularShader.standart = 1;


    if(!textShader.loadBinary(SHADER_BINARY_PATH"text.shad"))
    {
        textShader.attach(SHADER_SOURCE_PATH"text.vert", SHADER_VERTEX);
        textShader.attach(SHADER_SOURCE_PATH"text.frag", SHADER_FRAGMENT);
        textShader.bindAttribLocation("vertex", 0);
        textShader.bindAttribLocation("texCoord", 1);
        textShader.bindAttribLocation("color", 3);
        textShader.linkProgram();
    }
    textShader.validate();
    textShader.type = shader::shaderType::text;
    textShader.standart = 1;


    if(!guiShader.loadBinary(SHADER_BINARY_PATH"gui.shad"))
    {
        guiShader.attach(SHADER_SOURCE_PATH"gui.vert", SHADER_VERTEX);
        guiShader.attach(SHADER_SOURCE_PATH"gui.frag", SHADER_FRAGMENT);
        guiShader.bindAttribLocation("vertex", 0);
        guiShader.bindAttribLocation("texCoord", 1);
        guiShader.bindAttribLocation("color", 3);
        guiShader.linkProgram();
    }
    guiShader.validate();
    guiShader.type = shader::shaderType::gui;
    guiShader.standart = 1;

    guiShader.uniform1i("tex1", 0);
    guiShader.uniform1i("tex2", 1);


    if(!particlesShader.loadBinary(SHADER_BINARY_PATH"particles.shad"))
    {
        particlesShader.attach(SHADER_SOURCE_PATH"particles.vert", SHADER_VERTEX);
        particlesShader.attach(SHADER_SOURCE_PATH"particles.frag", SHADER_FRAGMENT);
        particlesShader.bindAttribLocation("vertex", 0);
        particlesShader.bindAttribLocation("texCoord", 1);
        particlesShader.bindAttribLocation("color", 2);
        particlesShader.linkProgram();
    }
    particlesShader.validate();
    particlesShader.type = shader::shaderType::particles;
    particlesShader.standart = 1;

    guiTextures[0].load("data/textures/buttonShape.png", 1);
    guiTextures[1].load("data/textures/buttonColor.png", 1);

    guiTextures[0].parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    guiTextures[0].parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    guiTextures[1].parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    guiTextures[1].parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    textFont.load("data/font");
    _log::out("=================end loading default resources================\n\n");
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
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(2);
        buffers2D.VBO[0].bind();

        glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(type2D::vertex), (GLuint*)(offsetof(type2D::vertex, vert))); //vertex
        glVertexAttribPointer(2, 4, GL_FLOAT, 0, sizeof(type2D::vertex), (GLuint*)(offsetof(type2D::vertex, color))); //color rgba
    }

    { //для треугольников
        buffers2D.VAO[1].bind();
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        buffers2D.VBO[1].bind();

        glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(type2D::texVert), (GLuint*)(offsetof(type2D::texVert, vert))); //vertex
        glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(type2D::texVert), (GLuint*)(offsetof(type2D::texVert, texCoord))); //texCoord
        glVertexAttribPointer(2, 4, GL_FLOAT, 0, sizeof(type2D::texVert), (GLuint*)(offsetof(type2D::texVert, color))); //color rgba
    }

    { //для частиц
        buffers2D.VAO[2].bind();
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        buffers2D.VBO[2].bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(type2D::particle), (GLuint*)(offsetof(type2D::particle, vert))); //vertex
        glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(type2D::particle), (GLuint*)(offsetof(type2D::particle, tex))); //texCoord
        glVertexAttribPointer(2, 4, GL_FLOAT, 0, sizeof(type2D::particle), (GLuint*)(offsetof(type2D::particle, color))); //color rgba
    }

    //*)
    //(* 3D
    {
        buffers3D.VAO[1].bind();
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        buffers3D.VBO[1].bind();

        glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(type3D::texVert), (GLuint*)(offsetof(type3D::texVert, vert))); //vertex
        glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(type3D::texVert), (GLuint*)(offsetof(type3D::texVert, texCoord))); //texCoord
        glVertexAttribPointer(2, 3, GL_FLOAT, 0, sizeof(type3D::texVert), (GLuint*)(offsetof(type3D::texVert, normal))); //normal
        glVertexAttribPointer(3, 4, GL_FLOAT, 0, sizeof(type3D::texVert), (GLuint*)(offsetof(type3D::texVert, color))); //color rgba
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
    buffers3D.VAO[2].unbind();
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

