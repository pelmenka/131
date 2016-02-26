#include <GL/glew.h>
#include <list>
#include "buffer.h"
#include "../log.h"

std::list<render::vertexBuffer*> VBOlist;

render::vertexBuffer *oldBuffer = 0;
render::vertexArray *oldArray = 0;

namespace render
{
vertexArray::vertexArray()
{
    obj = 0;
}

vertexArray::~vertexArray()
{
    free();
}

void vertexArray::gen()
{
    _alloc();
}

void vertexArray::_alloc()
{
    if(obj)
        return;
    glGenVertexArrays(1, &obj);
    _addToList();
}

void vertexArray::free()
{
    if(!obj)
        return;
    unbind();
    glDeleteVertexArrays(1, &obj);
    obj = 0;
    _removeFromList();
}

void vertexArray::bind()
{
    if(obj && oldArray != this)
    {
        glBindVertexArray(obj);
        oldArray = this;
    }
}

void vertexArray::unbind()
{
    glBindVertexArray(0);
    oldArray = 0;
}

void vertexArray::draw(uint mode, int first, size_t count)
{
    bind();
    glDrawArrays(mode, first, count);
    unbind();
}




///=============================================================

vertexBuffer::vertexBuffer()
{
    obj = 0;
    mode = GL_ARRAY_BUFFER;
}

vertexBuffer::~vertexBuffer()
{
    if(obj)
        free();
}

void vertexBuffer::gen()
{
    if(glIsBuffer(obj))
        return;
    glGenBuffers(1, &obj);
    VBOlist.push_back(this);
}

void vertexBuffer::free(bool end)
{
    if(!glIsBuffer(obj))
        return;
    unbind();
    glDeleteBuffers(1, &obj);
    obj = 0;
    if(!end) VBOlist.remove(this);
    oldBuffer = 0;
}

void vertexBuffer::bind(int m)
{
    if(m)
        mode = m;
    if(obj && oldBuffer != this)
    {
        glBindBuffer(mode, obj);
        oldBuffer = this;
    }
}

void vertexBuffer::unbind()
{
    glBindBuffer(mode, 0);
    oldBuffer = 0;
}

void vertexBuffer::createEmptyBuffer(uint size, uint usage)
{
    if(!obj)
        gen();
    bind();
    glBufferData(mode, size, 0, usage);
}

void vertexBuffer::_data(uint size, const void *data, uint usage)
{
    bind();
    glBufferData(mode, size, data, usage);
}

void vertexBuffer::_subData(size_t size, const void *data, int offset)
{
    bind();
    glBufferSubData(mode, offset, size, data);
}

void vertexBuffer::freeAll()
{
    for(auto i: VBOlist)
        i->free(1);

    VBOlist.clear();
}

}
