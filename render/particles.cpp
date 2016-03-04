#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <tinyxml2.h>
#include <list>
#include <random>
#include <vector>
#include <cmath>
#include <memory>
#include "particles.h"
#include "../color.h"
#include "../window.h"

float urandom();
float random();
vec2f random2();
vec3f random3();
vec4f random4();

void parseColor(const char*, vec3f*);

float mathEmitterParam(uint, const std::vector<render::emitterParameter<float>>&, float);

namespace render{

void emitter2D::update(const vec2f &pos)
{
    if(_data.empty())
        return;
    /*if(param.gravityAngle != _oldParam.gravityAngle
    || param.gravitySpeed != _oldParam.gravitySpeed)
    {
        _gravityModifier = {cos(param.gravityAngle*DTOR)*param.gravitySpeed, sin(param.gravityAngle*DTOR)*param.gravitySpeed};
        _oldParam.gravityAngle = param.gravityAngle;
        _oldParam.gravitySpeed = param.gravitySpeed;
    }*/

    float delta = glfwGetTime() - _oldTime;
    _oldTime = glfwGetTime();


    if(param.workTime+_startTime < glfwGetTime())
    {
        if(param.looped)
        {
            currentNodes = vec3f(0);
            _startTime = glfwGetTime();
        }
        else
            stop();
    }

    float val = glfwGetTime()-_startTime/param.workTime;

    if(param.workTime+_startTime >= glfwGetTime())
        _emissionCount += mathEmitterParam(currentNodes.z, emissionSpeed, val)*delta;
    if(_emissionCount > param.maxParticles)
        _emissionCount = param.maxParticles;
    float maxSpawn = _emissionCount;
    _drawCount = 0;
    for(uint i = 0; i < _data.size(); i++)
    {
        if(!_update(i, delta))
            if(_running
            && _emissionCount > 0)
            {
                float d = _emissionCount/maxSpawn;
                _spawn(i, pos*d+_oldPos*(1.0f-d));
            }
    }
    _oldPos = pos;
}

bool emitter2D::_update(uint c, float delta)
{
    if(_data[c].timer <= 0)
        return 0;
    _data[c].timer -= delta;
    /*if(_data[c].speed.maxNodes)
        for(int i = 0; i < _data[c].speed.maxNodes; i++)
            _data[c].speed.values[i].value += _gravityModifier*delta;*/

    float val = 1.0-_data[c].timer/_data[c].lifeTime;
    float ctimer;
    uint index;

    _partData[_drawCount].tex = _data[c].texCoord;

    _partData[_drawCount].color = _data[c].color.math(val);
    _partData[_drawCount].alpha = _data[c].alpha.math(val);

    _data[c].pos += _data[c].direction.math(val)*_data[c].speed.math(val)*delta;
    _partData[_drawCount].vert = vec3f(_data[c].pos.x, _data[c].pos.y, _data[c].scale.math(val));
    _drawCount++;
    return 1;
}

bool emitter2D::isRunning()
{
    return _running;
}

void emitter2D::_spawn(uint c, const vec2f &pos)
{
    _emissionCount--;

    float val = glfwGetTime()-_startTime/param.workTime;

    _data[c].timer = _data[c].lifeTime = mathEmitterParam(currentNodes.y, lifeTime, val);
    switch(param.spawnType)
    {
    case 0:
        _data[c].pos = param.pos+param.area*random2(); break;
    case 1:
        {float a = 3.1415*random();
        vec2f p;
        sincosf(a, &p.x, &p.y);
        p *= param.area*random();  //да, так и должно быть
        _data[c].pos = param.pos+p;//нет, делать распределение по двум осям не нужно, достаточно одной
        break;}
    default:
        _data[c].pos = param.pos; break;
    }
    if(_atlas)
    {
        if(!textureNodes.empty())
        {
            uint node = rand()%textureNodes.size();
            _data[c].texCoord = _atlas->getTexCoord(textureNodes[node]);
        }else
            _data[c].texCoord = vec4f(0, 0, 1, 1);
    }

    _data[c].pos += pos;

    ///================NODES

    _data[c].alpha.currentNode = 0;
    _data[c].color.currentNode = 0;
    _data[c].scale.currentNode = 0;
    _data[c].speed.currentNode = 0;
    _data[c].direction.currentNode = 0;

    _data[c].color.resize(particles.color.size());
    for(uint i = 0; i < particles.color.size(); i++)
    {
        _data[c].color.values[i].value = particles.color[i].value+particles.color[i].spread*random3();
        _data[c].color.values[i].time  = particles.color[i].time;
    }

    _data[c].alpha.resize(particles.alpha.size());
    for(uint i = 0; i < particles.alpha.size(); i++)
    {
        _data[c].alpha.values[i].value = particles.alpha[i].value+particles.alpha[i].spread*random();
        _data[c].alpha.values[i].time  = particles.alpha[i].time;
    }

    _data[c].scale.resize(particles.scale.size());
    for(uint i = 0; i < particles.scale.size(); i++)
    {
        _data[c].scale.values[i].value = particles.scale[i].value+particles.scale[i].spread*random();
        _data[c].scale.values[i].time  = particles.scale[i].time;
    }

    _data[c].direction.resize(particles.angle.size());
    for(uint i = 0; i < particles.angle.size(); i++)
    {
        float temp = particles.angle[i].value+particles.angle[i].spread*random()+mathEmitterParam(currentNodes.x, angle, val);
        sincosf(temp*DTOR, &_data[c].direction.values[i].value.y, &_data[c].direction.values[i].value.x);
        _data[c].direction.values[i].time = particles.angle[i].time;
    }

    _data[c].speed.resize(particles.speed.size());
    for(uint i = 0; i < particles.speed.size(); i++)
    {
        _data[c].speed.values[i].value = particles.speed[i].value+particles.speed[i].spread*random();
        _data[c].speed.values[i].time  = particles.speed[i].time;
    }

    _update(c, 0);
}

void emitter2D::draw()
{
    if(!_data.size() || !_drawCount)
        return;
    if(param.blendMode)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    else
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    if(_atlas)
        _atlas->bind();
    CHECK_DEF_SHAD(particles);
    render::_internal::_addParticles2D(_partData.data(), _drawCount);
}

void emitter2D::start()
{
    _data.resize(param.maxParticles);
    _partData.resize(param.maxParticles);

    _running = 1;
    _startTime = glfwGetTime();
    _oldTime = glfwGetTime();
    _nothingToDraw = 1;
    _emissionCount = 0;
    currentNodes = vec3f(0);
}

void emitter2D::stop()
{
    _running = 0;
}

void emitter2D::setTextureAtlas(textureAtlas *ptr)
{
    _atlas = ptr;
}

textureAtlas* emitter2D::getTextureAtlas()
{
    return _atlas;
}

emitter2D::emitter2D(effect2D *parent)
{
    _parent = parent;
    memset(&param, 0, sizeof(emitter2DParam));
    //_oldParam = param;
    param.maxParticles = 512;

    _running = 0;
    _drawCount = 1;
    _gravityModifier = {0, 0};
    _atlas = 0;
    _emissionCount = 0;

    particles.alpha.push_back(emitterParameter<float>(0, 0.5));
    particles.angle.push_back(emitterParameter<float>(0, 0));
    particles.color.push_back(emitterParameter<vec3f>(0, vec3f(0, 0, 1)));
    particles.scale.push_back(emitterParameter<float>(0, 5));
    particles.speed.push_back(emitterParameter<float>(0, 30));

    lifeTime.push_back(emitterParameter<float>(0, 2));
    emissionSpeed.push_back(emitterParameter<float>(0, 30));
    param.workTime = 5;
}

void emitter2D::clear()
{
    particles.alpha.clear();
    particles.angle.clear();
    particles.color.clear();
    particles.scale.clear();
    particles.speed.clear();

    lifeTime.clear();
    emissionSpeed.clear();
}

emitter2D::~emitter2D()
{
//    free();
}

///=================================================

effect2D::effect2D()
{
    pos = {0, 0};
}

effect2D::effect2D(const std::string &path)
{
    pos = {0, 0};
    load(path);
}

effect2D::~effect2D()
{
    free();
}

bool effect2D::load(const std::string &path)
{
    tinyxml2::XMLDocument doc;
    if(doc.LoadFile(path.c_str()))
        return 0;

    tinyxml2::XMLElement *root = doc.RootElement()->FirstChildElement("emitter");

    do
    {
        emitter2D *tempEmitter = createEmitter();
        tempEmitter->clear();
        tinyxml2::XMLElement *element, *nElement, *pElement;

        root->QueryAttribute("maxParticles", &tempEmitter->param.maxParticles);
        root->QueryAttribute("timer", &tempEmitter->param.workTime);
        root->QueryAttribute("looped", &tempEmitter->param.looped);
        tempEmitter->setTextureAtlas(0);
        if(element = root->FirstChildElement("atlas"))
        {
            const char *p = element->Attribute("src");
            if(p)
            {
                textureAtlas *atlas = new textureAtlas(p, 1);

                tempEmitter->setTextureAtlas(atlas);
                element = element->FirstChildElement("node");
                if(element)
                do
                {
                    uint n = 0;
                    element->QueryAttribute("pos",&n);
                    tempEmitter->textureNodes.push_back(n);
                }while(element = element->NextSiblingElement("node"));
            }
        }

        if(element = root->FirstChildElement("blend"))
            element->QueryAttribute("mode", &tempEmitter->param.blendMode);

        if(element = root->FirstChildElement("position"))
        {
            element->QueryAttribute("x", &tempEmitter->param.pos.x);
            element->QueryAttribute("y", &tempEmitter->param.pos.y);
        }
        if(element = root->FirstChildElement("spawn"))
        {
            element->QueryAttribute("x", &tempEmitter->param.area.x);
            element->QueryAttribute("y", &tempEmitter->param.area.y);
            element->QueryAttribute("mode", &tempEmitter->param.spawnType);
        }

        if(element = root->FirstChildElement("angle"))
        {
            uint nodeCount = 0;
            element->QueryAttribute("nodes", &nodeCount);
            tempEmitter->angle.reserve(nodeCount);

            for(nElement = element->FirstChildElement("node"); nElement;
                nElement = nElement->NextSiblingElement("node"))
            {
                emitterParameter<float> temp;
                nElement->QueryAttribute("time", &temp.time);
                nElement->QueryAttribute("angle", &temp.value);
                nElement->QueryAttribute("spread", &temp.spread);
                tempEmitter->angle.push_back(temp);
            }
        }

        if(element = root->FirstChildElement("life"))
        {
            uint nodeCount = 0;
            element->QueryAttribute("nodes", &nodeCount);
            tempEmitter->lifeTime.reserve(nodeCount);

            for(nElement = element->FirstChildElement("node"); nElement;
                nElement = nElement->NextSiblingElement("node"))
            {
                emitterParameter<float> temp;
                nElement->QueryAttribute("time", &temp.time);
                nElement->QueryAttribute("life", &temp.value);
                nElement->QueryAttribute("spread", &temp.spread);
                tempEmitter->lifeTime.push_back(temp);
            }
        }

        if(element = root->FirstChildElement("emission"))
        {
            uint nodeCount = 0;
            element->QueryAttribute("nodes", &nodeCount);
            tempEmitter->emissionSpeed.reserve(nodeCount);

            for(nElement = element->FirstChildElement("node"); nElement;
                nElement = nElement->NextSiblingElement("node"))
            {
                emitterParameter<float> temp;
                nElement->QueryAttribute("time", &temp.time);
                nElement->QueryAttribute("speed", &temp.value);
                nElement->QueryAttribute("spread", &temp.spread);
                tempEmitter->emissionSpeed.push_back(temp);
            }
        }

        pElement = root->FirstChildElement("particles");

            if(element = pElement->FirstChildElement("angle"))
            {
                uint nodeCount = 0;
                element->QueryAttribute("nodes", &nodeCount);
                tempEmitter->particles.angle.reserve(nodeCount);

                for(nElement = element->FirstChildElement("node"); nElement;
                    nElement = nElement->NextSiblingElement("node"))
                {
                    emitterParameter<float> temp;
                    nElement->QueryAttribute("time", &temp.time);
                    nElement->QueryAttribute("angle", &temp.value);
                    nElement->QueryAttribute("spread", &temp.spread);
                    tempEmitter->particles.angle.push_back(temp);
                }
            }

            if(element = pElement->FirstChildElement("speed"))
            {
                uint nodeCount = 0;
                element->QueryAttribute("nodes", &nodeCount);
                tempEmitter->particles.speed.reserve(nodeCount);

                for(nElement = element->FirstChildElement("node"); nElement;
                    nElement = nElement->NextSiblingElement("node"))
                {
                    emitterParameter<float> temp;
                    nElement->QueryAttribute("time", &temp.time);
                    nElement->QueryAttribute("speed", &temp.value);
                    nElement->QueryAttribute("spread", &temp.spread);
                    tempEmitter->particles.speed.push_back(temp);
                }
            }

            if(element = pElement->FirstChildElement("scale"))
            {
                uint nodeCount = 0;
                element->QueryAttribute("nodes", &nodeCount);
                tempEmitter->particles.scale.reserve(nodeCount);

                for(nElement = element->FirstChildElement("node"); nElement;
                    nElement = nElement->NextSiblingElement("node"))
                {
                    emitterParameter<float> temp;
                    nElement->QueryAttribute("time", &temp.time);
                    nElement->QueryAttribute("scale", &temp.value);
                    nElement->QueryAttribute("spread", &temp.spread);
                    tempEmitter->particles.scale.push_back(temp);

                }
            }

            if(element = pElement->FirstChildElement("alpha"))
            {
                uint nodeCount = 0;
                element->QueryAttribute("nodes", &nodeCount);
                tempEmitter->particles.alpha.reserve(nodeCount);

                for(nElement = element->FirstChildElement("node"); nElement;
                    nElement = nElement->NextSiblingElement("node"))
                {
                    emitterParameter<float> temp;
                    nElement->QueryAttribute("time", &temp.time);
                    nElement->QueryAttribute("alpha", &temp.value);
                    nElement->QueryAttribute("spread", &temp.spread);
                    tempEmitter->particles.alpha.push_back(temp);
                }
            }

            if(element = pElement->FirstChildElement("color"))
            {
                uint nodeCount = 0;
                element->QueryAttribute("nodes", &nodeCount);
                tempEmitter->particles.color.reserve(nodeCount);

                for(nElement = element->FirstChildElement("node"); nElement;
                    nElement = nElement->NextSiblingElement("node"))
                {
                    emitterParameter<vec3f> temp;
                    nElement->QueryAttribute("time", &temp.time);

                    parseColor(nElement->Attribute("color"), &temp.value);
                    parseColor(nElement->Attribute("spread"), &temp.spread);
                    tempEmitter->particles.color.push_back(temp);
                }
            }
    }while(root = root->NextSiblingElement("emitter"));

    return 1;
}

void effect2D::save(const std::string &path, bool compact)
{
    /*tinyxml2::XMLDocument doc;

    tinyxml2::XMLElement *root = doc.NewElement("effect");

    doc.InsertEndChild(root);

    for(emitter2D *i:_data)
    {
        char tempColor[64];
        root = doc.NewElement("emitter");
        doc.RootElement()->InsertEndChild(root);
        root->SetAttribute("maxParticles", i->param.maxParticles);
        root->SetAttribute("time", i->param.workTime);
        root->SetAttribute("looped", i->param.looped);

        tinyxml2::XMLElement *element, *spreadElement, *nodeElement, *partElement;

        if(i->_atlas)
        {
            root->InsertEndChild(element = doc.NewElement("atlas"));
            element->SetAttribute("src", i->_atlas->getName().c_str());
            for(auto j: i->textureNodes)
            {
                element->InsertEndChild(spreadElement = doc.NewElement("node"));
                spreadElement->SetAttribute("pos", j);
            }
        }

        root->InsertEndChild(element = doc.NewElement("blend"));
        element->SetAttribute("mode", i->param.blendMode);

        root->InsertEndChild(element = doc.NewElement("position"));
        element->SetAttribute("x", i->param.pos.x);
        element->SetAttribute("y", i->param.pos.y);

        root->InsertEndChild(element = doc.NewElement("spawn"));
        element->SetAttribute("x", i->param.area.x);
        element->SetAttribute("y", i->param.area.y);
        element->SetAttribute("mode", i->param.spawnType);

        //(* EMITTER PARAMETERS

            root->InsertEndChild(element = doc.NewElement("life"));
            element->SetAttribute("nodes", i->lifeTime.size());
            for(auto &j: i->lifeTime)
            {
                element->InsertEndChild(nodeElement = doc.NewElement("node"));
                nodeElement->SetAttribute("time", j.time);
                nodeElement->SetAttribute("life", j.value);
                nodeElement->SetAttribute("spread", j.spread);
            }

            root->InsertEndChild(element = doc.NewElement("emission"));
            element->SetAttribute("nodes", i->emissionSpeed.size());
            for(auto &j: i->emissionSpeed)
            {
                element->InsertEndChild(nodeElement = doc.NewElement("node"));
                nodeElement->SetAttribute("time", j.time);
                nodeElement->SetAttribute("speed", j.value);
                nodeElement->SetAttribute("spread", j.spread);
            }

            root->InsertEndChild(element = doc.NewElement("angle"));
            element->SetAttribute("nodes", i->angle.size());
            for(auto &j: i->angle)
            {
                element->InsertEndChild(nodeElement = doc.NewElement("node"));
                nodeElement->SetAttribute("time", j.time);
                nodeElement->SetAttribute("angle", j.value);
                nodeElement->SetAttribute("spread", j.spread);
            }

        //*)

        //(* PARTICLES PARAMETERS

            root->InsertEndChild(partElement = doc.NewElement("particles"));

            partElement->InsertEndChild(element = doc.NewElement("angle"));
            element->SetAttribute("nodes", i->particles.angle.size());
            for(auto &j: i->particles.angle)
            {
                element->InsertEndChild(nodeElement = doc.NewElement("node"));
                nodeElement->SetAttribute("time", j.time);
                nodeElement->SetAttribute("angle", j.value);
                nodeElement->SetAttribute("spread", j.spread);
            }

            partElement->InsertEndChild(element = doc.NewElement("scale"));
            element->SetAttribute("nodes", i->particles.scale.size());
            for(auto &j: i->particles.scale)
            {
                element->InsertEndChild(nodeElement = doc.NewElement("node"));
                nodeElement->SetAttribute("time", j.time);
                nodeElement->SetAttribute("scale", j.value);
                nodeElement->SetAttribute("spread", j.spread);
            }

            partElement->InsertEndChild(element = doc.NewElement("speed"));
            element->SetAttribute("nodes", i->particles.speed.size());
            for(auto &j: i->particles.speed)
            {
                element->InsertEndChild(nodeElement = doc.NewElement("node"));
                nodeElement->SetAttribute("time", j.time);
                nodeElement->SetAttribute("speed", j.value);
                nodeElement->SetAttribute("spread", j.spread);
            }

            partElement->InsertEndChild(element = doc.NewElement("alpha"));
            element->SetAttribute("nodes", i->particles.alpha.size());
            for(auto &j: i->particles.alpha)
            {
                element->InsertEndChild(nodeElement = doc.NewElement("node"));
                nodeElement->SetAttribute("time", j.time);
                nodeElement->SetAttribute("alpha", j.value);
                nodeElement->SetAttribute("spread", j.spread);
            }

            partElement->InsertEndChild(element = doc.NewElement("color"));
            element->SetAttribute("nodes", i->particles.color.size());
            for(auto &j: i->particles.color)
            {
                element->InsertEndChild(nodeElement = doc.NewElement("node"));
                nodeElement->SetAttribute("time", j.time);
                sprintf(tempColor, "%f;%f;%f", j.value.x, j.value.y, j.value.z);
                nodeElement->SetAttribute("color", tempColor);
                sprintf(tempColor, "%f;%f;%f", j.spread.x, j.spread.y, j.spread.z);
                nodeElement->SetAttribute("spread", tempColor);
            }

        //*)
    }
    doc.InsertFirstChild(doc.NewDeclaration());
    doc.SaveFile(path.c_str(), compact);*/
}

void effect2D::update()
{
    for(emitter2D*i:_data)
        i->update(pos);
}

void effect2D::draw()
{
    for(emitter2D*i:_data)
        i->draw();
}

void effect2D::stop()
{
    for(emitter2D*i:_data)
        i->stop();
}

void effect2D::start()
{
    for(emitter2D*i:_data)
        i->start();
}

void effect2D::free()
{
    for(emitter2D*i:_data)
        delete i;
    _data.clear();
}

emitter2D* effect2D::createEmitter()
{
    emitter2D* temp = new emitter2D;
    _data.push_back(temp);
    return temp;
}

emitter2D* effect2D::_getEmitter(uint i)
{
    try
    {
        return _data.at(i);
    }catch(...)
    {
        return nullptr;
    }
}

///================================================

template<typename T>
void _particleParam<T>::resize(size_t s)
{
    if(s == size)
        return;
    delete [] values;
    size = s;
    if(!s)
    {
        values = 0;
        return;
    }
    values = new node[size];
}

template<typename T>
_particleParam<T>::_particleParam()
{
    values = 0;
    size = 0;
}

template<typename T>
_particleParam<T>::~_particleParam()
{
    if(values)
        delete [] values;
}


template<typename T>
T _particleParam<T>::math(float a)
{
    if(currentNode == size || size == 1)
        return values[currentNode].value;

    if(values[currentNode].time > a)
        return values[currentNode].value;

    if(values[currentNode+1].time < a)
        if(++currentNode == size-1)
            return values[currentNode].value;

    float ctimer = (a-values[currentNode].time)/(values[currentNode+1].time-values[currentNode].time);
    return mix(values[currentNode+1].value, values[currentNode].value, ctimer);
}

}
///================================================

float mathEmitterParam(uint id, const std::vector<render::emitterParameter<float>> &param, float timer)
{
    if(param.empty())
        return 2;
    if(id >= param.size()-1 || param.size() == 1)
        return param[id].value+param[id].spread*random();

    if(param[id].time > timer)
        return param[id].value+param[id].spread*random();

    if(param[id+1].time < timer)
        if(++id == param.size()-1)
            return param[id].value+param[id].spread*random();

    float ctimer = (timer-param[id].time)/(param[id+1].time-param[id].time);
    return mix(param[id+1].value+param[id].spread*random(),
               param[id].value+param[id].spread*random(), ctimer);
}

void parseColor(const char *str, vec3f *out)
{
    if(!str || !out) return;
    sscanf(str, "%f;%f;%f", &out->x, &out->y, &out->z);
}

float urandom()
{
    return float(rand())/RAND_MAX;
}

float random()
{
    return urandom()*2.0f-1.0f;
}

vec3f random3()
{
    return vec3f(random(), random(), random());
}

vec2f random2()
{
    return vec2f(random(), random());
}
