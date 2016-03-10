#include <glm/gtc/matrix_transform.hpp>
#include <cstring>
#include "render.h"
#include "model.h"

vec3f readVertices(const char*);
vec2f readTexCoords(const char*);
vec3f readNormals(const char*);
void readIndecies(std::vector<vec3i>&, const char*);


namespace render{

model::model():_test(*this)
{
    visible = 1;
}

bool model::load(const std::string &path)
{
    FILE *f = fopen(path.c_str(), "rt");
    if(!f)
        return 0;
    const char *keyWords[] = {"v", "vt", "vn", "f", "o", "usemtl", "mtllib"};
    std::vector<vec3f> vertices;
    std::vector<vec2f> texCoords;
    std::vector<vec3f> normals;
    std::vector<vec3i> indecies;
    while(!feof(f))
    {
        char buffer[256], nameBuffer[8];
        if(!fgets(buffer, 256, f))
            break;
        sscanf(buffer, "%s ", nameBuffer);
        for(int i = 0; i < 7; i++)
            if(!strcmp(nameBuffer, keyWords[i]))
            {
                switch(i)
                {
                case 0:
                    vertices.push_back(readVertices(buffer)); break;
                case 1:
                    texCoords.push_back(readTexCoords(buffer)); break;
                case 2:
                    normals.push_back(readNormals(buffer)); break;
                case 3:
                    readIndecies(indecies, buffer+1); break;
                }
                break;
            }
    }
    _test._data = new type3D::texVert[indecies.size()];
    _test._count = indecies.size();
    for(uint i = 0; i < indecies.size(); i++)
    {
        vec3i &index = indecies[i];
        if(index.x)
            _test._data[i].vert = vertices[index.x-1];
        if(index.y)
            _test._data[i].texCoord = texCoords[index.y-1];
        if(index.z)
            _test._data[i].normal = normals[index.z-1];
        _test._data[i].color = vec4f(1);
    }

    fclose(f);
    return 1;
}

void model::draw()
{
    if(!visible)
        return;
    glm::mat4 mat = glm::translate(glm::mat4(1), pos);
    mat = glm::rotate(mat, rotate.x, glm::vec3(1, 0, 0));
    mat = glm::rotate(mat, rotate.y, glm::vec3(0, 1, 0));
    mat = glm::rotate(mat, rotate.z, glm::vec3(0, 0, 1));
    _test.draw(mat);
}

mech::mech(const model &m):_parent(m)
{
    visible = 1;
    _data = 0;
}

mech::~mech()
{
    if(_data)
        delete [] _data;
}

void mech::draw(glm::mat4 mat)
{
    if(!visible)
        return;
    endDraw();
    mat = glm::translate(mat, pos);
    mat = glm::rotate(mat, rotate.x, glm::vec3(1, 0, 0));
    mat = glm::rotate(mat, rotate.y, glm::vec3(0, 1, 0));
    mat = glm::rotate(mat, rotate.z, glm::vec3(0, 0, 1));

    defaultResources::regular3DShader.bind();
    defaultResources::regular3DShader.uniform4x4("modelMatrix", 1, &mat[0][0]);
    _internal::_addTriangles3D(_data, _count);
}

}

vec3f readVertices(const char *str)
{
    vec3f temp;
    sscanf(str, "v %f %f %f", &temp.x, &temp.y, &temp.z);
    return temp;
}

vec2f readTexCoords(const char *str)
{
    vec2f temp;
    sscanf(str, "vt %f %f", &temp.x, &temp.y);
    return temp;
}

vec3f readNormals(const char *str)
{
    vec3f temp;
    sscanf(str, "vn %f %f %f", &temp.x, &temp.y, &temp.z);
    return temp;
}

void readIndecies(std::vector<vec3i> &arr, const char *str)
{
    uint verts = 0;
    std::vector<uint> pos;
    std::vector<vec3i> data;
    for(int i = 0; str[i] != '\n'; i++)
        if(str[i] == ' ')
        {
            pos.push_back(i);
            verts++;
        }
    data.reserve(verts);
    for(auto &i: pos)
    {
        vec3i temp;
        sscanf(str+i, "%d/%d/%d", &temp.x, &temp.y, &temp.z);
        data.push_back(temp);
    }
    for(int i = 2; i < verts; i++)
    {
        arr.push_back(data[0]);
        arr.push_back(data[i-1]);
        arr.push_back(data[i]);
    }
}
