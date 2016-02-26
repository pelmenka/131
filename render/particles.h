#ifndef PARTICLES_H_INCLUDED
#define PARTICLES_H_INCLUDED

#include "../common.h"
#include <vector>
#include <string>
#include "buffer.h"
#include "atlas.h"

namespace render{

template<typename T>
struct _particleParam
{
    uint currentNode;

    struct node
    {
        float time;
        T value;
    };
    std::vector<node> values;

    T math(float a)
    {
        if(currentNode == values.size()-1 || values.size() == 1)
            return values[currentNode].value;

        if(values[currentNode].time > a)
            return values[currentNode].value;

        if(values[currentNode+1].time < a)
            if(++currentNode == values.size()-1)
                return values[currentNode].value;

        float ctimer = (a-values[currentNode].time)/(values[currentNode+1].time-values[currentNode].time);
        return mix(values[currentNode+1].value, values[currentNode].value, ctimer);
    }
};

template<typename T>
struct emitterParameter
{
    emitterParameter():time(0),value(0),spread(0){}
    emitterParameter(float _time, T _val):time(_time),value(_val),spread(0){}
    emitterParameter(float _time, T _val, T _spread):time(_time),value(_val),spread(_spread){}

    float time;
    T value, spread;
};

class effect2D;
class emitter2D;

struct _particle2D
{
    _particleParam<vec3f> color;
    _particleParam<float> alpha;
    _particleParam<float> scale;
    _particleParam<float> speed;
    _particleParam<vec2f> direction;

    vec2f pos;
    float lifeTime, timer;
    vec4f texCoord;
};

struct emitter2DParam
{
    vec2f pos;
    vec2f area;
    uint spawnType;
    float gravityAngle, gravitySpeed;
    float workTime;
    bool looped;
    uint blendMode;
    uint maxParticles;
};

class emitter2D
{
public:
    emitter2DParam param;

    emitter2D(effect2D *parent = 0);
    ~emitter2D();

    void start();
    void stop();

    void setTextureAtlas(textureAtlas*);
    textureAtlas* getTextureAtlas();

    void update(const vec2f& = {0, 0});
    void draw();

    void clear();

    bool isRunning();

protected:
    bool _update(uint, float);
    void _spawn(uint, const vec2f&);
private:
    effect2D *_parent;
    textureAtlas *_atlas;
    std::vector<type2D::particle> _partData;
    std::vector<_particle2D> _data;
    float _startTime, _oldTime;
    bool _running, _nothingToDraw;
    vec2f _gravityModifier;
    float _emissionCount;

    uint _drawCount;
    friend class effect2D;
    vec3u currentNodes;

public:
    struct
    {
        std::vector<emitterParameter<vec3f>> color;
        std::vector<emitterParameter<float>> alpha;
        std::vector<emitterParameter<float>> scale;
        std::vector<emitterParameter<float>> angle;
        std::vector<emitterParameter<float>> speed;
    }particles;

    std::vector<emitterParameter<float>> angle;
    std::vector<emitterParameter<float>> lifeTime;
    std::vector<emitterParameter<float>> emissionSpeed;

    std::vector<uint> textureNodes;
};

class effect2D
{
public:
    vec2f pos;

    effect2D();
    effect2D(const std::string&);
    ~effect2D();

    void update();
    void draw();

    void start();
    void stop();
    void free();

    bool load(const std::string&);
    void save(const std::string&, bool compact = 0);

    emitter2D* createEmitter();

    emitter2D& operator[](uint i){return *_getEmitter(i);}
private:
    //effect2DParam _param;
    vec3f _rgbModifier;
    emitter2D* _getEmitter(uint);
    std::vector<emitter2D*> _data;
};

}

#endif // PARTICLES_H_INCLUDED
