#include <GL/glew.h>
#include <stdio.h>
#include <memory.h>
#include <list>
#include "shader.h"
#include "render.h"
#include "../log.h"

enum shaderError
{
    no_file = 0,
    empty_file,
    compile_error,
    linking_error
};

namespace render{

shader::shader()
{
    shaders[0] = 0;
    shaders[1] = 0;
    program = 0;
    standart = 0;
    type = shaderType::regular;
}

shader::shader(const char* vert, const char* frag, shaderType _type, bool _standart):shader()
{
    load(vert, frag, _type, _standart);
}

shader::~shader()
{
    if(program)
        free();
}

int shader::getUniformLocation(const std::string &name)
{
    bind();
    for(auto &i: _uniformLoc)
        if(i.name == name)
            return i.loc;
    int loc = glGetUniformLocation(program, name.c_str());
    _uniformLoc.push_back(_uniform(loc, name));
    return loc;
}

bool shader::addFromFile(bool shad, const char* path)
{
    const char *names[] = {"vertex", "fragment"};
    FILE *src;
    char *source;
    _log::out("Loading %s shader: %s ", names[shad], path);
    try
    {
        uint types[2] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
        int length;

        if(!(src = fopen(path, "rt")))
            throw "Failed to open file";

        fseek(src, 0, 2);
        if(!(length = ftell(src)))
            throw "File is empty";
        fseek(src, 0, 0);

        length;
        source = new char[length+1];
        memset(source, 0, length+1);
        fread(source, 1, length, src);

        if(glIsShader(shaders[shad]))
        {
            unbind();
            glDetachShader(program, shaders[shad]);
        }
        else
            shaders[shad] = glCreateShader(types[shad]);

        const char *s = source;
        glShaderSource(shaders[shad], 1, &s, 0);
        glCompileShader(shaders[shad]);
        delete [] source;
        if(!_compileOk(shaders[shad]))
            throw "Failed to compile shader";
    }
    catch(const char* s)
    {
        _log::out("[FAIL]\n%s\n", s);
        if(src)
            fclose(src);
        return 0;
    }
    fclose(src);

    _log::out("[ OK ]\n");

    return 1;
}

bool shader::addFromCode(bool shad, const char* source)
{
    const char *names[] = {"vertex", "fragment"};
    _log::out("Compiling %s shader from code -> ", names[shad]);
    try
    {
        uint types[2] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
        if(glIsShader(shaders[shad]))
        {
            unbind();
            glDetachShader(program, shaders[shad]);
        }
        else
            shaders[shad] = glCreateShader(types[shad]);

        const char *s = source;
        glShaderSource(shaders[shad], 1, &s, 0);
        glCompileShader(shaders[shad]);

        if(!_compileOk(shaders[shad]))
            throw "Failed to compile shader";
    }
    catch(const char* s)
    {
        _log::out("[FAIL]\n%s\n", s);
        return 0;
    }

    _log::out("[ OK ]\n");

    return 1;
}

void shader::compile()
{
    if(!glIsProgram(program))
        program = glCreateProgram();

    glAttachShader(program, shaders[0]);
    glAttachShader(program, shaders[1]);
    glLinkProgram(program);
    int lenth;

    _log::out("Linking shader program ");
    glGetProgramiv(program, GL_LINK_STATUS, &lenth);
    if(!lenth)
    {
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &lenth);
        if(lenth)
        {
            char *mess = new char[lenth];
            glGetProgramInfoLog(program, lenth, 0, mess);
            _log::out("[FAIL]\n%s\n", mess);
            delete [] mess;
            return;
        }
    }
    _alloc();
    _log::out("[ OK ]\n");
}

void shader::_alloc()
{
    _addToList();
}

bool shader::load(const char* vert, const char* frag, shaderType _type, bool _standart)
{
    if(addFromFile(0, vert) && addFromFile(1, frag))
    {
        standart = _standart;
        type = _type;
        compile();
        return 1;
    }
    return 0;
}

bool shader::loadFromCode(const char* vert, const char* frag)
{
    if(addFromCode(0, vert) && addFromCode(1, frag))
    {
        compile();
        return 1;
    }
    return 0;
}

void shader::bind()
{
    if(render::getActiveShader() == this) return;
    render::_internal::_bindShader(this);
    glUseProgram(program);
}

void shader::unbind() const
{
    if(!render::getActiveShader()) return;
    glUseProgram(0);
    render::_internal::_bindShader(0);
}

bool shader::_compileOk(GLuint i)
{
    int log = 0;
    glGetShaderiv(i, GL_COMPILE_STATUS, &log);
    return log;
}

bool shader::_linkOk(GLuint i)
{
    int log = 0;
    glGetShaderiv(i, GL_LINK_STATUS, &log);
    return log;
}

void shader::_printError(GLuint i)
{
    int log = 0;
    glGetShaderiv(i, GL_INFO_LOG_LENGTH, &log);
    char *mess = new char[log];
    glGetShaderInfoLog(i, log, 0, mess);
    _log::out("\t%s\n", mess);
    delete [] mess;
}

void shader::free()
{
    unbind();
    if(glIsProgram(program))
    {
        glDetachShader(program, shaders[0]);
        glDetachShader(program, shaders[1]);
        glDeleteProgram(program);
        program = 0;
    }

    for(int i = 0; i < 2; i++)
        if(glIsShader(shaders[i]))
        {
            glDeleteShader(shaders[i]);
            shaders[i] = 0;
        }
    _removeFromList();
}

void shader::uniform1f(const std::string &name, float v1)
{
    endDraw();
    glUniform1f(getUniformLocation(name), v1);
}

void shader::uniform2f(const std::string &name, float v1, float v2)
{
    endDraw();
    glUniform2f(getUniformLocation(name), v1, v2);
}

void shader::uniform3f(const std::string &name, float v1, float v2, float v3)
{
    endDraw();
    glUniform3f(getUniformLocation(name), v1, v2, v3);
}

void shader::uniform4f(const std::string &name, float v1, float v2, float v3, float v4)
{
    endDraw();
    glUniform4f(getUniformLocation(name), v1, v2, v3, v4);
}

void shader::uniform1fv(const std::string &name, uint count, const float *v)
{
    endDraw();
    glUniform1fv(getUniformLocation(name), count, v);
}

void shader::uniform2fv(const std::string &name, uint count, const float *v)
{
    endDraw();
    glUniform2fv(getUniformLocation(name), count, v);
}

void shader::uniform3fv(const std::string &name, uint count, const float *v)
{
    endDraw();
    glUniform3fv(getUniformLocation(name), count, v);
}

void shader::uniform4fv(const std::string &name, uint count, const float *v)
{
    endDraw();
    glUniform4fv(getUniformLocation(name), count, v);
}

///=================================================

void shader::uniform1i(const std::string &name, int v1)
{
    endDraw();
    glUniform1i(getUniformLocation(name), v1);
}

void shader::uniform2i(const std::string &name, int v1, int v2)
{
    endDraw();
    glUniform2i(getUniformLocation(name), v1, v2);
}

void shader::uniform3i(const std::string &name, int v1, int v2, int v3)
{
    endDraw();
    glUniform3i(getUniformLocation(name), v1, v2, v3);
}

void shader::uniform4i(const std::string &name, int v1, int v2, int v3, int v4)
{
    endDraw();
    glUniform4i(getUniformLocation(name), v1, v2, v3, v4);
}

void shader::uniform1iv(const std::string &name, uint count, const int *v)
{
    endDraw();
    glUniform1iv(getUniformLocation(name), count, v);
}

void shader::uniform2iv(const std::string &name, uint count, const int *v)
{
    endDraw();
    glUniform2iv(getUniformLocation(name), count, v);
}

void shader::uniform3iv(const std::string &name, uint count, const int *v)
{
    endDraw();
    glUniform3iv(getUniformLocation(name), count, v);
}

void shader::uniform4iv(const std::string &name, uint count, const int *v)
{
    endDraw();
    glUniform4iv(getUniformLocation(name), count, v);
}

}
