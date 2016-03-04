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
}

hero::~hero()
{
    delete armory[0];
    delete armory[1];
}

void hero::update()
{
    static float _oldTime = 0;

    speed = 600;
    vec2d cursor = input::getCursor();
    angle = atan2(pos.x-cursor.x, pos.y-cursor.y);
    velocity = vec2f();
    if(input::getKey('S')) velocity.y = 1;
    if(input::getKey('W')) velocity.y = -1;
    if(input::getKey('A')) velocity.x = -1;
    if(input::getKey('D')) velocity.x = 1;
    if(input::getKey(0))
        armory[1]->shot();
    armory[1]->update();


    if(velocity.x != 0 && velocity.y != 0)
        velocity *= sinf(45);
    velocity *= speed*window::deltaTime;
    pos += velocity;
}

void hero::draw() const
{
    //render::camera.pos = -pos+vec2f(80);
    resources::hero.angle = -angle;
    resources::hero.draw(pos);
    //render::primitives2d::circle(pos, 32, Hex2RGBA(0xf44336), -angle*RTOD, 4);
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
    if(time > 0)
        time -= window::deltaTime;
}

void pistol::update()
{
    weapon::update();
    if(pressed)
        pressed--;
}

void pistol::shot()
{
    if(!pressed && time <= 0)
        level::makeShot(owner->getPos(), owner->getAngle()+3.1415, 800, 1);
    pressed = 2;
}

vec2f random2();

void shotgun::shot()
{
    if(!pressed && time <= 0)
    {
        for(int i = 0; i <= 6; i++)
        {
            vec2f r = random2();
            level::makeShot(owner->getPos(), owner->getAngle()+3.1415+r.x*0.2, 800, 0.2+r.y*.1);
        }
    }
    pressed = 2;
}

///=====================

enemy::enemy()
{
    health = -1;
}

void enemy::spawn(const vec2f &p, float s, float h)
{
    pos = p;
    speed = s;
    health = h;
}

void enemy::update(hero *h)
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
    }
    else time -= window::deltaTime;
}

void enemy::hit(float damage)
{
    health -= damage;
    if(health < 0)
        time = 1;
}

void enemy::draw()
{
    if(health > 0)
        render::primitives2d::circle(pos, 32, Hex2RGBA(0xf44336), -angle*RTOD, 4);
    else if(time > 0)
        render::primitives2d::circle(pos, 32, Hex2RGBA(0xf44336, time), -angle*RTOD, 4);
}

bool enemy::isAlive()
{
    return health > 0 || time > 0;
}

bool enemy::AABB()
{

}
