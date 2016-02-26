#include <tinyxml2.h>
#include "font.h"

using namespace tinyxml2;

namespace render{


font::font()
{

}

font::font(const std::string &path):font()
{
    load(path);
}

bool font::load(const std::string &path)
{
    XMLDocument doc;

    std::string tempPath = path+".png";

    if(!texture::load(tempPath, 1, 1))
        return 0;

    tempPath = path+".xml";

    if(doc.LoadFile(tempPath.c_str()))
    {
        free();
        return 0;
    }

    uint size = 1;

    XMLElement *fontElement = doc.RootElement()->FirstChildElement("font");

    if(sscanf(fontElement->Attribute("size"), "%dpt", &size))
        defaultScale = 12.0/size;
    else if(sscanf(fontElement->Attribute("size"), "%dpx", &size))
        defaultScale = 16.0/size;
    else
        defaultScale = 1;

    for(XMLElement *element = doc.RootElement()->FirstChildElement()->FirstChildElement("char"); element;
        element = element->NextSiblingElement("char"))
        {
            _charInfo tempInfo;
            vec4i tempCoord;
            element->QueryAttribute("id", &tempInfo.code);
            element->QueryAttribute("x", &tempCoord.x);
            element->QueryAttribute("y", &tempCoord.y);
            element->QueryAttribute("width", &tempCoord.z);
            element->QueryAttribute("height", &tempCoord.w);

            tempInfo.imgSize.x = tempCoord.z;
            tempInfo.imgSize.y = tempCoord.w;

            tempInfo.texCoord.x = float(tempCoord.x)/texture::_size.x;
            tempInfo.texCoord.y = float(tempCoord.y)/texture::_size.y;
            tempInfo.texCoord.z = float(tempCoord.z)/texture::_size.x;
            tempInfo.texCoord.w = float(tempCoord.w)/texture::_size.y;

            element->QueryAttribute("Xoffset", &tempInfo.offset.x);
            element->QueryAttribute("Yoffset", &tempInfo.offset.y);

            element->QueryAttribute("OrigWidth", &tempInfo.size.x);
            element->QueryAttribute("OrigHeight", &tempInfo.size.y);

            if(!tempInfo.size.x)
                tempInfo.size.x = tempCoord.x;

            if(!tempInfo.size.y)
                tempInfo.size.y = tempCoord.y;

            _charList.push_back(tempInfo);
        }
    uint oldCode = 0, id;

    for(XMLElement *element = doc.RootElement()->FirstChildElement()->FirstChildElement("kerning"); element;
        element = element->NextSiblingElement("kerning"))
        {
            _kerningInfo temp;
            uint code = 0;
            element->QueryAttribute("first", &code);
            if(!code) continue;
            if(code != oldCode)
            {
                oldCode = 0;
                for(int i = 0; i < _charList.size(); i++)
                    if(_charList[i].code == code)
                    {
                        oldCode = code;
                        id = i;
                    }
            }

            if(!oldCode)
                continue;

            element->QueryAttribute("second", &temp.code);
            element->QueryAttribute("value", &temp.offset);
            _charList[id].kerning.push_back(temp);
        }
}

void font::draw(const drawGlyphInfo *data, uint count, float scale) const
{
    if(!count)
        return;
    const vec2f offset[6] = {{0, 1}, {1, 1}, {1, 0},
                             {1, 0}, {0, 0}, {0, 1}};

    type3D::texVert buffer[0xfff];
    uint counter = 0;

    scale *= defaultScale;
    for(int c = 0; c < count; c++)
    {
        if(data[c].code > _charList.size())
            continue;

        _charInfo temp = _charList[data[c].code];

        for(int i = 0; i < 6; i++)
        {
            buffer[counter].vert = vec3f(data[c].pos+(offset[i]*temp.imgSize+temp.offset)*scale, scale);

            buffer[counter].texCoord.x = (offset[i].x*temp.texCoord.z)+temp.texCoord.x;
            buffer[counter].texCoord.y = 1.0-(offset[i].y*temp.texCoord.w)-temp.texCoord.y;

            buffer[counter].color = data[c].color;
            counter++;
        }
    }
    render::defaultResources::textShader.bind();
    bind();
    render::_internal::_addTriangles3D(buffer, counter);
}

}
