#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <list>
#include <string>
#include "object.h"
#include "../common.h"

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
        gui = 4
    };

    bool standart;

    shaderType type;

    shader();
    shader(const char*, const char*, shaderType = shaderType::regular, bool = 0);
    ~shader();

    void bind();
    void unbind() const;

    bool load(const char*, const char*, shaderType = shaderType::regular, bool = 0);
    bool loadFromCode(const char*, const char*);

    uint getProgram() const;

    int getUniformLocation(const std::string&);

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

    void free();
private:
    void _alloc();
    static bool _compileOk(uint i);
    static bool _linkOk(uint i);
    static void _printError(uint i);
    uint shaders[2], program;
    bool addFromFile(bool, const char*);
    bool addFromCode(bool, const char*);
    void compile();
    struct _uniform{int loc; std::string name; _uniform(int l, const std::string &n):loc(l), name(n){}};
    std::list<_uniform> _uniformLoc;
};

}

#endif // SHADER_H_INCLUDED
