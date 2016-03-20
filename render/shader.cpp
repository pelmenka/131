#include <GL/glew.h>
#include <cstdio>
#include <memory.h>
#include "shader.h"
#include "render.h"
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
    if(!_linked)
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
    program = glCreateProgram();
    _addToList();
}

bool shader::attach(const std::string &path, uint s)
{
    if(_linked)
        return 0;
    if(s > SHADER_GEOMETRY)
        return 0;

    if(!program)
        _alloc();

    uint types[3] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER};
    const char *names[] = {"vertex", "fragment", "geometry"};
    _log::out("Loading %s shader: %s ", names[s], path.c_str());
    FILE *src;
    try
    {
        int length;

        if(!(src = fopen(path.c_str(), "rt")))
            throw "Failed to open file";

        fseek(src, 0, 2);
        if(!(length = ftell(src)))
            throw "File is empty";
        fseek(src, 0, 0);
        GLchar *source = new char[length+1];
        memset(source, 0, length+1);
        fread(source, 1, length, src);

        if(!glIsShader(shaders[s]))
            shaders[s] = glCreateShader(types[s]);
        else
            if(_attachedShaders&(1<<s))
            {
                glDetachShader(program, shaders[s]);
                _attachedShaders ^= (1<<s);
            }
        const char *txt = source;
        glShaderSource(shaders[s], 1, &txt, 0);
        glCompileShader(shaders[s]);
        delete [] source;

        int status = 0;
        glGetShaderiv(shaders[s], GL_COMPILE_STATUS, &status);
        if(!status)
            throw "Compile Error";

        glAttachShader(program, shaders[s]);
        _attachedShaders |= (1<<s);
        fclose(src);
    }
    catch(const char* msg)
    {
        _log::out("[FAIL]\n%s\n", msg);
        _printSError(shaders[s]);
        if(src)
            fclose(src);
        return 0;
    }
    _log::out("[ OK ]\n");
    return 1;
}

bool shader::linkProgram(bool binaryHint)
{
    if(_linked)
        return 0;
    if(binaryHint)
        glProgramParameteri(program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
    glLinkProgram(program);
    int lenth;

    _log::out("Linking shader program ");
    glGetProgramiv(program, GL_LINK_STATUS, &lenth);
    if(!lenth)
    {
        _log::out("[FAIL]\n");
        _printPError(program);
        return 0;
    }
    _log::out("[ OK ]\n");
    _linked = 1;
    return 1;
}

bool shader::validate()
{
    if(!_linked) return 0;
    glValidateProgram(program);
    int s = -1;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &s);
    _log::out("Validate shader %s\n", (s==1)?"[ OK ]":"[FAIL]");
    return s;
}

bool shader::loadBinary(const std::string &path)
{
    if(_linked)
        return 0;
    int formatsCount = 0, length = 0;
    GLenum format;
    _log::out("loading binary shader: %s ", path.c_str());
    FILE *f;
    try
    {
        if(!(f = fopen(path.c_str(), "rb")))
            throw "Failed to open file";
        fseek(f, 0, 2);
        if(!(length = ftell(f)))
            throw "File is empty";
        fseek(f, 0, 0);
        length -= sizeof(GLenum);
        char *data = new char[length];
        fread(&format, sizeof(GLenum), 1, f);
        fread(data, length, 1, f);

        _alloc();
        glProgramBinary(program, format, data, length);
        delete [] data;
        glGetProgramiv(program, GL_LINK_STATUS, &length);
        if(!length)
            throw "Some error";
        _linked = 1;
        fclose(f);
    }
    catch(const char* msg)
    {
        _log::out("[FAIL]\n%s\n", msg);
        _printPError(program);
        if(f)
            fclose(f);
        return 0;
    }
    _log::out("[ OK ]\n");
    return 1;
}

bool shader::saveBinary(const std::string &path)
{
    if(!_linked)
        return 0;
    int length = 0;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &length);
    if(!length)
        return 0;
    char *data = new char[length];
    GLenum format;
    glGetProgramBinary(program, length, 0, &format, data);
    FILE *out = fopen(path.c_str(), "wb");
    if(out)
    {
        fwrite(&format, sizeof(GLenum), 1, out);
        fwrite(data, length, 1, out);
        fclose(out);
    }
    _linked = 0;
    delete [] data;
    return 1;
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

void shader::_printSError(GLuint i)
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

void shader::_printPError(GLuint i)
{
    int log = 0;
    glGetProgramiv(i, GL_INFO_LOG_LENGTH, &log);
    if(!log)
        return;
    char *mess = new char[log];
    glGetProgramInfoLog(i, log, 0, mess);
    _log::out("\t%s\n", mess);
    delete [] mess;
}

void shader::free()
{
    unbind();
    if(glIsProgram(program))
    {
        for(int i = 0 ; i < 3; i++)
            if(_attachedShaders&(1<<i))
                glDetachShader(program, shaders[i]);
        glDeleteProgram(program);
        program = 0;
        _attachedShaders = 0;
    }
    for(int i = 0; i < 3; i++)
        if(glIsShader(shaders[i]))
        {
            glDeleteShader(shaders[i]);
            shaders[i] = 0;
        }
    _removeFromList();
}

void shader::loadProjectionMatrix(const glm::mat4 &mat)
{
    uniform4x4(projectionMatrixName, 1, &mat[0][0]);
}

void shader::loadModelMatrix(const glm::mat4 &mat)
{
    uniform4x4(modelMatrixName, 1, &mat[0][0]);
}

void shader::loadViewMatrix(const glm::mat4 &mat)
{
    uniform4x4(viewMatrixName, 1, &mat[0][0]);
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
