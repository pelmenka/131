#include "level.h"
#include "hero.h"
#include "render/particles.h"
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

    vec2f pos, _oldPos;
private:
    render::icon sprite;
    render::effect2D &blood;
    float angle;
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
    enemies[0].spawn(vec2f(800), 400, 5000);
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
            for(auto &j: enemies)
                if(j.isAlive())
                {
                    if(j.collide(i.pos))
                        i.hit(&j);
                }
        }
    resources::bloodEffect.update();
}

void drawBullets()
{
    for(auto &i: bulletsList)
        i.draw();
    resources::bloodEffect.draw();
}


bullet::bullet():blood(resources::bloodEffect)
{
    //blood = resources::bloodEffect;
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
    //sprite.color = rgba(1, 0.9, 0.6, 1);
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

void bullet::hit(enemy *en)
{
    time = -1;
    en->hit(1, speed*0.04);
    blood[0].param.pos = pos;
    blood[0].particles.angle[0].value = -angle*RTOD+90;
    blood[0].start();
    return;
}

bool bullet::isAlive() const
{
    return time > 0;
}
