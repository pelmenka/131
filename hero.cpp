#include "render/primitives.h"
#include "resources.h"
#include "hero.h"
#include "window.h"
#include "input.h"
#include "color.h"
#include <cmath>
#include "level.h"

hero::hero()
{
    armory[0] = new pistol(this);
    armory[1] = new shotgun(this);
    armory[2] = new machinegun(this);
    currentGun = armory[0];
    ammo = 99;

    sprite = resources::entities.getIcon("hero.png");
}

hero::~hero()
{
    delete armory[0];
    delete armory[1];
    delete armory[2];
}

void hero::update()
{
    static float _oldTime = 0;
    speed = 600;
    vec2d cursor = input::getCursor();
    angle = atan2(pos.x-cursor.x-render::camera.pos.x, pos.y-cursor.y-render::camera.pos.y);
    velocity = vec2f();
    if(input::getKey('S')) velocity.y = 1;
    if(input::getKey('W')) velocity.y = -1;
    if(input::getKey('A')) velocity.x = -1;
    if(input::getKey('D')) velocity.x = 1;

    for(int i = 0; i < 3; i++)
        if(input::getKey('1'+i))
            currentGun = armory[i];

    if(input::getKey('R'))
        currentGun->reload();

    if(input::getKey(0))
        currentGun->shot();
    currentGun->update();


    if(velocity.x != 0 && velocity.y != 0)
        velocity *= sinf(45);
    velocity *= speed*window::deltaTime;
    pos += velocity;
    if(pos.x < 0)
        pos.x = 0;
    if(pos.y < 0)
        pos.y = 0;
    if(pos.x > arenaX*40)
        pos.x = arenaX*40;
    if(pos.y > arenaY*40)
        pos.y = arenaY*40;
}

void hero::draw()
{
    sprite.angle = -angle;
    sprite.draw(pos);
}

uint hero::getAmmo(uint a)
{
    if(!a){return ammo;}
    if(ammo < a)
    {
        a = ammo;
        ammo = 0;
        return a;
    }
    ammo -= a;
    return a;
}

void hero::addAmmo(uint a)
{
    ammo += a;
    if(ammo > 256)
        ammo = 256;
}

const weapon *hero::getCurrentGun()
{
    return currentGun;
}

vec3u hero::getArsenalState()
{
    vec3u temp;
    for(int i = 0; i < 3; i++)
    {
        if(currentGun == armory[i])
            temp[i] |= 2;
        if(armory[i]->getAmmo().x)
            temp[i] |= 1;
    }
    return temp;
}

float hero::getAngle() const noexcept
{
    return angle;
}

const vec2f &hero::getPos() const noexcept
{
    return pos;
}



///=====================

weapon::weapon(hero *o)
{
    owner = o;
    delay = 0.1;
    time = 0;
}

void weapon::update()
{
    if(!ammo.x)
    {
        if(_reload < window::deltaTime)
        {
            _reload = 0;
            ammo.x = owner->getAmmo(ammo.y);
        }
        else if(owner->getAmmo(0))
            _reload -= window::deltaTime;
    }
    if(time > 0)
        time -= window::deltaTime;
}

void weapon::reload()
{
    owner->addAmmo(ammo.x);
    ammo.x = 0;
    _reload = reloadTime;
}

vec2u weapon::getAmmo() const noexcept
{
    return ammo;
}

vec2f weapon::getReloadTime() const noexcept
{
    return vec2f(_reload, reloadTime);
}

void pistol::update()
{
    weapon::update();
    if(pressed)
        pressed--;
}

void pistol::shot()
{
    if(!pressed && time <= 0 && ammo.x)
    {
        level::makeShot(owner->getPos(), owner->getAngle()+3.1415, speed, 1, damage);
        if(!(--ammo.x))
            _reload = reloadTime;
    }

    pressed = 2;
}

vec2f random2();

void shotgun::shot()
{
    if(!pressed && time <= 0 && ammo.x)
    {
        for(int i = 0; i <= 6; i++)
        {
            vec2f r = random2();
            level::makeShot(owner->getPos(), owner->getAngle()+3.1415+r.x*0.2, speed, 0.2+r.y*.1, damage);
        }
        if(!(--ammo.x))
            _reload = reloadTime;
    }
    pressed = 2;
}

void machinegun::shot()
{
    if(time <= 0 && ammo.x)
    {
        level::makeShot(owner->getPos(), owner->getAngle()+3.1415, speed, 1, damage);
        if(!(--ammo.x))
            _reload = reloadTime;
    }

}

///=====================

bool triCollide(const vec2f&, const vec2f&, const vec2f&, const vec2f&);

enemy::enemy(const vec2f &p, float s, float h)
{
    pos = p;
    speed = s;
    health = h;
    sprite = resources::entities.getIcon(rand()%3);
}

bool enemy::update(hero *h)
{
    if(health > 0)
    {
        vec2f hpos = pos-h->getPos();
        angle = atan2(hpos.x, hpos.y);
        vec2f velocity;
        sincosf(angle, &velocity.x, &velocity.y);
        velocity *= window::deltaTime*speed;

        if(hpos < velocity)
            pos = h->getPos();
        else
            pos -= velocity;
        return 1;
    }
    time -= window::deltaTime;
    return 0;
}

void enemy::hit(float damage, const vec2f &dir)
{
    health -= damage;
    pos += dir;
    if(health <= 0)
        time = 1;
}

void enemy::draw()
{
    if(health > 0)
    {
        sprite.angle = -angle;
        sprite.draw(pos);
    }
    else if(time > 0)
    {
        sprite.color.w = time;
        sprite.draw(pos);
    }
}

inline bool enemy::isAlive() const noexcept
{
    return health > 0;
}

bool enemy::collide(const vec2f &s, const vec2f &e) const
{
    if(!isAlive())
        return 0;
    vec2f vert[6];
    memcpy(vert, sprite.getVertices(), sizeof(vec2f)*6);
    for(int i = 0; i < 6; i++)
        vert[i] += pos;
    vec2f p = s-e;
    int iters = ceil(p.length()/16)+1;
    p /= iters;

    for(int i = 0; i < iters; i++)
        if(triCollide(s+p*i, vert[0], vert[1], vert[2]) || triCollide(p+p*i, vert[3], vert[4], vert[5]))
            return 1;
    return 0;
}
///===================================================

cooler::cooler()
{
    time = 5;
}

void cooler::update() noexcept
{
    if(time <= 0)
    {
        alive = 1;
        if(pos == targetPos)
        {
            targetPos = vec2f(rand()%(arenaX*40), rand()%(arenaY*40));
            vec2f c = targetPos - pos;
            float angle = atan2(c.x, c.y);
            sincosf(angle, &velocity.x, &velocity.y);
            velocity *= speed;
        }
        vec2f tpos = targetPos - pos;
        if(tpos < velocity*window::deltaTime)
        {
            pos = targetPos;
        }else
        pos += velocity*window::deltaTime;
        resources::coolEffect.pos = pos;
    }else
    {
        alive = 0;
        time -= window::deltaTime;
        if(time <= 0)
            resources::coolEffect.start();
    }
    resources::coolEffect.update();
}

bool cooler::isCollide(const vec2f &p) noexcept
{
    if(!alive)
        return 0;
    vec2f t = pos - p;
    if(t < vec2f(20))
    {
        time = 30;
        resources::coolEffect.stop();
        return 1;
    }

    return 0;
}

void cooler::draw() const noexcept
{
    resources::coolEffect.draw();
}

///===================================================
bool triCollide(const vec2f &p, const vec2f &A, const vec2f &B, const vec2f &C)
{
    vec3f r;
    r.x = (A.x-p.x)*(B.y-A.y)-(B.x-A.x)*(A.y-p.y);
    r.y = (B.x-p.x)*(C.y-B.y)-(C.x-B.x)*(B.y-p.y);
    r.z = (C.x-p.x)*(A.y-C.y)-(A.x-C.x)*(C.y-p.y);

    if((r.x < 0 && r.y < 0 && r.z < 0)
    || (r.x ==0 && r.y ==0 && r.z ==0)
    || (r.x > 0 && r.y > 0 && r.z > 0))
        return 1;
    return 0;
}
