#ifndef _RENDER_BUFFER_H_INCLUDED
#define _RENDER_BUFFER_H_INCLUDED

#include "../common.h"
#include "object.h"

namespace render{

class vertexArray:_object
{

public:
    vertexArray();
    ~vertexArray();

    void gen();
    void free();

    void bind();
    void unbind();

    void draw(uint, int, uint);
private:
    uint obj;
    void _alloc();
};

class vertexBuffer:_object
{
public:
    vertexBuffer();
    ~vertexBuffer();

    void gen();
    void free();

    void bind(int = 0);
    void unbind();

    template<typename T>
    void data(uint count, const T* data, uint usage = 0x88E8)
    {
        _data(count*sizeof(T), data, usage);
    }

    template<typename T>
    void subData(uint count, const T* data, int offset = 0)
    {
        _subData(count*sizeof(T), data, offset);
    }

    void createEmptyBuffer(uint size, uint usage = 0x88E8);

    static void freeAll();
private:
    uint obj, mode;

    void _data(uint, const void*, uint);
    void _subData(uint, const void*, int);
    void _alloc();
};
}

#endif // BUFFER_H_INCLUDED
