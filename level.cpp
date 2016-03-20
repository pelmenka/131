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

    void spawn(vec2f, float, float, float, float);
    bool update();
    void draw();
    void hit(enemy*);

    bool isAlive() const;

    vec2f pos, _oldPos;
private:
    render::icon sprite;
    render::effect2D &blood;
    float angle;
    float damage;
    vec2f speed;
    float time;
};

const uint bulletsListSize = 128;

bullet bulletsList[bulletsListSize];
std::list<enemy*> enemies;

void updateBullets();
void drawBullets();

void spawnEnemy();

double deltaMultiplier = 1.0;

float spawnTime;

uchar data[arenaY][arenaX];

hero *actor;

cooler cool;

void level::init()
{
    for(int i = 0; i < arenaY; i++)
        for(int j = 0; j < arenaX; j++)
            data[i][j] = rand()%5;
    actor = new hero();
    spawnTime = 0.0;
}

void level::close()
{
    while(!enemies.empty())
    {
        enemy *e = enemies.back();
        delete e;
        enemies.pop_back();
    }
    delete actor;
}

void level::update()
{
    vec2u border = window::getSize()/2;
    render::camera.pos = vec3f(actor->getPos()-border, 0);


    window::deltaTime *= deltaMultiplier;
    actor->update();

    auto i = enemies.begin();
    while(i != enemies.end())
    {
        enemy *e = *i;
        if(!e->update(actor))
        {
            delete e;
            i = enemies.erase(i);
        }
        else;
            i++;

    }

    cool.update();

    if(cool.isCollide(actor->getPos()))
    {
        deltaMultiplier -= 1.5;
        if(deltaMultiplier < 0.5)
            deltaMultiplier = 0.5;
    }

    if(deltaMultiplier < 0.8)
    {
        deltaMultiplier += window::deltaTime;
        if(deltaMultiplier > 0.8)
            deltaMultiplier = 0.8;
    }

    spawnTime -= window::deltaTime;

    if(spawnTime <= 0)
    {
        spawnEnemy();
        spawnTime = 4.0;
    }

    updateBullets();
}

void level::draw()
{
    for(int i = 0; i < arenaY; i++)
        for(int j = 0; j < arenaX; j++)
            resources::tiles.draw(vec2f(j*40, i*40), vec2u(data[i][j], 0));

    cool.draw();

    actor->draw();
    drawBullets();
    for(auto i: enemies)
        i->draw();
}

void level::makeShot(vec2f pos, float angle, float speed, float time, float damage)
{
    for(auto &i: bulletsList)
        if(!i.isAlive())
        {
            i.spawn(pos, angle, speed, time, damage);
            break;
        }
}

void spawnEnemy()
{
    vec2f pos;
    switch(rand()%4)
    {
    case 0:
        pos = vec2f(rand()%(arenaX*40), 0); break;
    case 1:
        pos = vec2f(0, rand()%(arenaY*40)); break;
    case 2:
        pos = vec2f(rand()%(arenaX*40), arenaY*40); break;
    case 3:
        pos = vec2f(arenaX*40, rand()%(arenaY*40)); break;
    }
    enemy *temp = new enemy(pos, 400, 8);
    enemies.push_back(temp);
}

void updateBullets()
{
    for(auto &i: bulletsList)
        if(i.isAlive())
        {
            i.update();
            for(auto j: enemies)
                if(j->isAlive())
                {
                    if(j->collide(i._oldPos, i.pos))
                        i.hit(j);
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
    time = -1;
}

void bullet::spawn(vec2f p, float a, float s, float t, float d)
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
    damage = d;
}

bool bullet::update()
{
    if(time <= 0)
        return 1;
    _oldPos = pos;
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
    en->hit(damage, speed*0.04);
    blood[0].param.pos = pos;
    blood[0].particles.angle[0].value = -angle*RTOD+90;
    blood[0].start();
    if(deltaMultiplier < 2)
        deltaMultiplier += 0.01;
    return;
}

bool bullet::isAlive() const
{
    return time > 0;
}
