#include "level.h"
#include "hero.h"
#include "resources.h"
#include "window.h"
#include <cmath>

class bullet
{
public:
    bullet();

    void spawn(vec2f, float, float, float);
    bool update();
    void draw();
    void hit(enemy*);

    bool isAlive() const;
private:
    render::icon sprite;
    float angle;
    vec2f pos, _oldPos;
    vec2f speed;
    float time;
};

const uint bulletsListSize = 128;
const uint maxEnemies = 16;

bullet bulletsList[bulletsListSize];
enemy enemies[maxEnemies];

void updateBullets();
void drawBullets();

void level::init()
{
    enemies[0].spawn(vec2f(800), 400, 5);
}

void level::update(hero* h)
{
    for(auto &i: enemies)
        i.update(h);
    updateBullets();
}

void level::draw()
{
    drawBullets();
    for(auto &i: enemies)
        i.draw();
}

void level::makeShot(vec2f pos, float angle, float speed, float time)
{
    for(auto &i: bulletsList)
        if(!i.isAlive())
        {
            i.spawn(pos, angle, speed, time);
            break;
        }
}

void updateBullets()
{
    for(auto &i: bulletsList)
        if(i.isAlive())
        {
            i.update();
        }
}

void drawBullets()
{
    for(auto &i: bulletsList)
        i.draw();
}


bullet::bullet()
{
    time = -1;

}

void bullet::spawn(vec2f p, float a, float s, float t)
{
    sprite = resources::bullets.getIcon(0);
    sprite.scale = vec2f(2);
    time = t;
    pos = p;
    _oldPos = p;
    sincosf(a, &speed.x, &speed.y);
    angle = a;
    speed *= s;
    sprite.angle = -angle;
    sprite.color = rgba(1, 0.9, 0.6, 1);
}

bool bullet::update()
{
    if(time <= 0)
        return 1;

    if(time < window::deltaTime)
        pos += speed*time;
    else
        pos += speed*window::deltaTime;
    time -= window::deltaTime;
}

void bullet::draw()
{
    if(time <= 0)
        return;
    sprite.draw(pos);
}

bool bullet::isAlive() const
{
    return time > 0;
}
