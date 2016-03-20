#pragma once
#include "common.h"
#include "render/atlas.h"

class weapon;

class hero
{
public:
    hero();
    ~hero();

    void update();
    void draw();


    void addAmmo(uint);
    uint getAmmo(uint);

    float getAngle() const noexcept;
    const vec2f &getPos() const noexcept;

    const weapon *getCurrentGun();
    vec3u getArsenalState();
private:
    render::icon sprite;
    uint ammo;
    vec2f pos;
    float angle;
    float speed;
    vec2f velocity;
    weapon *armory[3];
    weapon *currentGun;
};

class enemy
{
public:
    enemy(const vec2f &pos, float speed, float health);

    inline bool isAlive() const noexcept;

    bool collide(const vec2f&, const vec2f&) const;

    void hit(float, const vec2f&);
    bool update(hero*);
    void draw();

    vec2f getPos(){return pos;}
private:
    render::icon sprite;
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
    vec2f getReloadTime() const noexcept;
    vec2u getAmmo() const noexcept;
    void reload();
protected:
    hero *owner;
    float reloadTime, _reload;
    float speed;
    float damage;
    float delay;
    float time;
    vec2u ammo;
};

class pistol:public weapon
{
public:
    pistol(hero *o):weapon(o){pressed = 0; ammo = vec2u(12); reloadTime = 1; delay = 0.2; damage = 3; speed = 1800;};
    void update();
    void shot();
protected:
    int pressed;
};

class shotgun:public pistol
{
public:
    shotgun(hero *o):pistol(o){ammo = vec2u(6); reloadTime = 1.5; delay = 0; damage = 2; speed = 1000;};
    void shot();
};

class machinegun:public weapon
{
public:
    machinegun(hero *o):weapon(o){ammo = vec2u(30);  reloadTime = 3; delay = 0.1; damage = 1.4; speed = 1500;};
    void shot();
};

class cooler
{
    const float speed = 150.0f;
public:

    cooler();

    void moveTo(const vec2f&) noexcept;
    bool isCollide(const vec2f&) noexcept;

    void update() noexcept;
    void draw() const noexcept;
private:
    bool alive;
    float time;
    vec2f velocity;
    vec2f pos;
    vec2f targetPos;
};

