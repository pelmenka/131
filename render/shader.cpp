#include <GL/glew.h>
#include <cstdio>
#include <memory.h>
#include "shader.h"
#include "../log.h"

namespace render{

shader::shader()
{
    projectionMatrixName = "projectionMatrix";
    viewMatrixName = "viewMatrix";
    modelMatrixName = "modelMatrix";
    _attachedShaders = 0;
    program = 0;
    standart = 0;
    _linked = 0;
    type = shaderType::regular;
}

shader::~shader()
{
    free();
}

int shader::getUniformLocation(const std::string &name)
{
    if(!linked)
        return -1;
    bind();
    for(auto &i: _uniformLoc)
        if(i.name == name)
            return i.loc;
    int loc = glGetUniformLocation(program, name.c_str());
    _uniformLoc.push_back(_uniform(loc, name));
    return loc;
}

void shader::_alloc()
{
    _addToList();
}

bool shader::attach(const std::string &path, uint s)
{
    if(_linked)
        return;
    if(s > SHADER_GEOMETRY)
        return;

    if(!program)
        program = glCreateProgram();

    uint types[3] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER};
    const char *names[] = {"vertex", "fragment", "geometry"};
    _log::out("Loading %s shader: %s ", names[s], path.c_str());
    try
    {
        int length;

        if(!(src = fopen(path.c_str(), "rt")))
            throw "Failed to open file";

        fseek(src, 0, 2);
        if(!(length = ftell(src)))
            throw "File is empty";
        fseek(src, 0, 0);
        source = new char[length+1];
        memset(source, 0, length+1);
        fread(source, 1, length, src);

        if(!glIsShader(shaders[s]))
            shaders[s] = glCreateShader(types[s]);
        else
            glDetachShader(program, shaders[s]);

        const char *s = source;
        glShaderSource(shaders[s], 1, &s, 0);
        glCompileShader(shaders[s]);
        delete [] source;

        int status = 0;
        glGetShaderiv(shaders[s], GL_COMPILE_STATUS, status);
        if(!log)
            throw "Compile Error";

        glAttachShader(program, shaders[s]);

        fclose(src);
    }
    catch(const char* s)
    {
        _log::out("[FAIL]\n%s\n", s);
        _printError(shaders[shad]);
        if(src)
            fclose(src);
        return 0;
    }
    _log::out("[ OK ]\n");
    return 1;
}

bool shader::linkProgram()
{
    if(_linked)
        return;
    glLinkProgram(program);
    int lenth;

    _log::out("Linking shader program ");
    glGetProgramiv(program, GL_LINK_STATUS, &lenth);
    if(!lenth)
    {
        _log::out("[FAIL]\n", mess);
        _printError(program);
        return 0;
    }
    _log::out("[ OK ]\n");
    _linked = 1;
    return 1;
}

bool shader::validate()
{
    if(!_linked) return;
    glValidateProgram(program);
    int s = -1;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &s);
    _log::out("Validate shader %s\n", (s==1)?"[ OK ]":"[FAIL]");
    return s;
}

void shader::bind()
{
    if(!_linked) return;
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
    if(!log)
        return;
    char *mess = new char[log];
    glGetShaderInfoLog(i, log, 0, mess);
    _log::out("\t%s\n", mess);
    delete [] mess;
}

void shader::free()
{
    /*unbind();
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
        }*/
    _removeFromList();
}

void shader::bindAttribLocation(const std::string &name, uint val)
{
    glBindAttribLocation(program, val, name.c_str());
}

uint shader::getAttribLocation(const std::string &name)
{
    if(_linked)
        return glGetAttribLocation(program, name.c_str());
    return -1;
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

void shader::uniform4x4(const std::string &name, uint count, const float *v)
{
    glUniformMatrix4fv(getUniformLocation(name), count, 0, v);
}

}
