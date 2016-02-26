#include <tinyxml2.h>
#include "font.h"
#include "../../render/resources.h"

using namespace tinyxml2;

namespace gui{


font::font()
{
    _data = new render::type2D::texVert[DATA_BUFFER_SIZE];
    _drawCount = 0;
}

font::font(std::string path):font()
{
    load(path);
}

bool font::load(std::string path)
{
    XMLDocument doc;

    std::string tempPath = path+".png";

    if(!_tex.load(tempPath, 1, 1))
        return 0;

    tempPath = path+".xml";

    if(doc.LoadFile(tempPath.c_str()))
    {
        _tex.free();
        return 0;
    }

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

            tempInfo.texCoord.x = float(tempCoord.x)/_tex.getSize().x;
            tempInfo.texCoord.y = float(tempCoord.y)/_tex.getSize().y;
            tempInfo.texCoord.z = float(tempCoord.z)/_tex.getSize().x;
            tempInfo.texCoord.w = float(tempCoord.w)/_tex.getSize().y;

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

void font::draw(const drawGlyphInfo *data, uint count, float scale)
{
    const vec2f offset[6] = {{0, 1}, {1, 1}, {1, 0},
                             {1, 0}, {0, 0}, {0, 1}};

    for(int c = 0; c < count; c++)
    {
        if(_drawCount+6 > DATA_BUFFER_SIZE)
            flush(scale);

        if(data[c].code > _charList.size())
            continue;

        _charInfo &temp = _charList[data[c].code];

        for(int i = 0; i < 6; i++)
        {
            _data[_drawCount].vert = data[c].pos+(offset[i]*temp.imgSize+temp.offset)*scale;

            _data[_drawCount].texCoord.x = (offset[i].x*temp.texCoord.z)+temp.texCoord.x;
            _data[_drawCount].texCoord.y = 1.0-(offset[i].y*temp.texCoord.w)-temp.texCoord.y;

            _data[_drawCount].color = data[c].color;
            _drawCount++;
        }
    }
    flush(scale);
}

void font::flush(float scale)
{
    if(!_drawCount)
        return;
    _tex.bind();

    _resource::textShader.bind();
    _resource::textShader.uniform1f("scale", scale);
    render::primitives2d::drawTrianglesFromBuffer(_data, _drawCount);
    _drawCount = 0;
    _resource::textShader.unbind();
}

}
