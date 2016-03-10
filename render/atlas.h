#ifndef ATLAS_H_INCLUDED
#define ATLAS_H_INCLUDED

#include "texture.h"
#include "render.h"
#include "primitives.h"
#include <vector>

namespace render{

struct textureAtlasNode
{
    float angle, _angle;
    vec2f scale;
    vec2f center, _center;
    vec2f rotMatrix[6];
    vec2f pos, size;
    vec4f color;

    inline bool operator!=(const textureAtlasNode &a) const noexcept{
        return a.angle != angle ||
               a.scale != scale ||
               a.center!= center||
               a.pos   != pos   ||
               a.size  != size  ||
               a.color != color;}
};

class textureAtlas;

class icon: private textureAtlasNode
{
    friend class textureAtlas;
public:
    using textureAtlasNode::angle;
    using textureAtlasNode::scale;
    using textureAtlasNode::center;
    using textureAtlasNode::color;
    const textureAtlas *parent() const;
    icon():_parent(0){};
    void draw(const vec2f& = vec2f());
    const vec2f& getSize() const;
    const vec2f *getVertices() const noexcept;
private:
    textureAtlas *_parent;
};

class textureAtlas:public texture
{
public:
    textureAtlas();
    textureAtlas(const std::string&, bool = SMOOTH_BY_DEFAULT);
    ~textureAtlas();

    bool load(const std::string&, bool = SMOOTH_BY_DEFAULT);

    void draw(const vec2f&, uint);
    void draw(const vec2f&, icon&);

    icon getIcon(uint);

    const vec4f &getTexCoord(uint) const;
    inline textureAtlasNode &operator[](uint a){return elements.at(a);}
private:

    std::vector<textureAtlasNode> elements;
    void mathRotate(textureAtlasNode&);
    void mathRotate(icon&);
};

}

#endif // ATLAS_H_INCLUDED
