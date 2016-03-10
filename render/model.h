#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include "texture.h"
#include "type.h"

namespace render
{
class model;
struct material
{
    std::string name;
    texture diffuse, normal, specular;
    vec4f color;
};

class mech
{
friend class model;
public:
    bool visible;

    glm::vec3 rotate;
    glm::vec3 pos;

    mech(const model&);
    ~mech();

    void draw(glm::mat4);
private:
    type3D::texVert *_data;
    size_t _count;
    const model &_parent;
};

class model
{
public:
    bool visible;

    glm::vec3 rotate;
    glm::vec3 pos;

    model();
    //~model();

    bool load(const std::string&);
    void draw();
private:
    mech _test;
    std::vector<material> materials;
    std::vector<mech> _objects;
    bool mathmtl(const std::string&, const std::string&);
    int findmtl(const std::string&);
};
}

