#pragma once
#include "common.h"

class weapon;

class hero
{
public:
    hero();
    ~hero();

    void update();
    void draw() const;

    float getAngle() const noexcept;
    const vec2f &getPos() const noexcept;
private:
    vec2f pos;
    float angle;
    float speed;
    vec2f velocity;
    weapon *armory[2];
};

class enemy
{
public:
    enemy();

    bool isAlive();

    bool collide(const vec2f&, const vec2f&) const;

    void hit(float);
    void spawn(const vec2f &pos, float speed, float health);
    void update(hero*);
    void draw();
private:
    bool AABB(const vec2f&, const vec2f&) const;
    float time;
    float health;
    vec2f pos;
    float angle;
    float speed;
};

class weapon
{
public:
    weapon(hero*);

    virtual void update();
    virtual void shot() = 0;
    void draw() const;
    float getReloadTime() const;
protected:
    hero *owner;
    float damage;
    float delay;
    float time;
    uint sprite;
};

class pistol:public weapon
{
public:
    pistol(hero *o):weapon(o){pressed = 0;};
    void update();
    void shot();
protected:
    int pressed;
};

class shotgun:public pistol
{
public:
    shotgun(hero *o):pistol(o){};
    void shot();
};

class machinegun:public weapon
{
public:
    pistol(hero *o):weapon(o){pressed = 0;};
    void update();
    void shot();
};
