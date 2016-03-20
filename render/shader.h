#pragma once

#include <glm/mat4x4.hpp>
#include <list>
#include <string>
#include "object.h"
#include "../common.h"

#define SHADER_VERTEX   0
#define SHADER_FRAGMENT 1
#define SHADER_GEOMETRY 2

#define SHADER_SOURCE_PATH "data/shaders/source/"
#define SHADER_BINARY_PATH "data/shaders/binary/"

namespace render{

class shader:_object
{
public:
    enum class shaderType
    {
        regular = 0,
        primitives = 1,
        particles = 2,
        text = 3,
        gui = 4,
        regular3D
    };

    bool standart;

    std::string projectionMatrixName, modelMatrixName, viewMatrixName;

    shaderType type;

    shader();
    ~shader();

    void bind();
    void unbind() const;

    bool attach(const std::string&, uint);
    bool linkProgram(bool binaryHint = 0);
    bool validate();

    bool loadBinary(const std::string&);
    bool saveBinary(const std::string&);

    void free();

    uint getProgram() const;

    int getUniformLocation(const std::string&);

    void bindAttribLocation(const std::string&, uint);
    uint getAttribLocation(const std::string&);

    void loadProjectionMatrix(const glm::mat4&);
    void loadModelMatrix(const glm::mat4&);
    void loadViewMatrix(const glm::mat4&);

    void uniform1f(const std::string&, float);
    void uniform2f(const std::string&, float, float);
    void uniform3f(const std::string&, float, float, float);
    void uniform4f(const std::string&, float, float, float, float);

    void uniform1fv(const std::string&, uint, const float*);
    void uniform2fv(const std::string&, uint, const float*);
    void uniform3fv(const std::string&, uint, const float*);
    void uniform4fv(const std::string&, uint, const float*);

    void uniform1i(const std::string&, int);
    void uniform2i(const std::string&, int, int);
    void uniform3i(const std::string&, int, int, int);
    void uniform4i(const std::string&, int, int, int, int);

    void uniform1iv(const std::string&, uint, const int*);
    void uniform2iv(const std::string&, uint, const int*);
    void uniform3iv(const std::string&, uint, const int*);
    void uniform4iv(const std::string&, uint, const int*);

    void uniform4x4(const std::string&, uint, const float*);
private:
    bool _needLoadProjMatrix, _needLoadViewMatrix, _linked;
    void _alloc();
    static void _printSError(uint i);
    static void _printPError(uint i);
    uint shaders[3], program, _attachedShaders;
    struct _uniform{int loc; std::string name; _uniform(int l, const std::string &n):loc(l), name(n){}};
    std::list<_uniform> _uniformLoc;
};

}
