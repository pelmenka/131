#include "resources.h"
#include "gui/gui.h"
#include "window.h"
#include "level.h"
#include "color.h"
#include "hud.h"

gui::progressBar healthBar, ammoBar, hotBar;
gui::button guns[3];

namespace hud{

void init()
{
    guns[0].icon = resources::guns.getIcon("five_seven.png");
    guns[1].icon = resources::guns.getIcon("benelli_m3.png");
    guns[2].icon = resources::guns.getIcon("ak47.png");
    for(int i = 0; i < 3; i++)
    {
        guns[i].regularColor = rgba(0);
        guns[i].size = vec2f(100, 50);
        guns[i].icon.color = rgba(0.5, 0.5, 0.5, 1);
    }
    healthBar.size = vec2f(200, 25);
    ammoBar.size = vec2f(200, 25);
    hotBar.size = vec2f(200, 55);

    healthBar.pos = vec2f(5);
    ammoBar.pos = vec2f(5, 35);
    hotBar.pos = vec2f(210, 5);

    healthBar.backcolor = rgba(.3, .3, .3, .7);
    healthBar.frontcolor = rgba(0.7, 0, 0, .7);
    ammoBar.backcolor = rgba(.3, .3, .3, .7);
    ammoBar.frontcolor = rgba(1);
    hotBar.backcolor = rgba(.3, .3, .3, .7);
    hotBar.value = 1;
    hotBar.range = 2.3;

    healthBar.value = 50;
}

// 200 = cool
// 120 = normal
// 10  = hot

void update()
{
    vec2i size = window::getSize();
    vec3u state = actor->getArsenalState();
    for(int i = 0; i < 3; i++)
    {
        guns[i].icon.color = rgba(1);
        if(!(state[i]&2))
            guns[i].icon.color *= rgba(.5, .5, .5, 1);
        if(!(state[i]&1))
            guns[i].icon.color *= rgba(1, 0, 0, 1);
        guns[i].pos = vec2f(size.x-105*(3-i), 5);
    }
    if(actor->getCurrentGun()->getAmmo().x)
    {
        vec2u a = actor->getCurrentGun()->getAmmo();
        ammoBar.range = a.y;
        ammoBar.value = a.x;
    }
    else
    {
        vec2f a = actor->getCurrentGun()->getReloadTime();
        ammoBar.range = a.y;
        ammoBar.value = a.y-a.x;
    }
    vec3f c(0.7);
    if(deltaMultiplier < 1)
        c.x = 200+160*(0.5-deltaMultiplier);
    else
    {
        c.x = 120-110*(deltaMultiplier-1.0);
    }
    hotBar.frontcolor = hsv2RGBA(c);
    hotBar.value = deltaMultiplier;
}

void draw()
{
    render::text ammoText;
    render::primitives2d::quad(vec2f(), vec2f(window::getSize().x, 65), rgba(.3, .3, .3, .7));
    for(auto &i: guns)
        i.draw();
    healthBar.draw();
    ammoBar.draw();
    hotBar.draw();
    ammoText.format("%d/256", actor->getAmmo(0));
    ammoText.scale = 2.5;
    ammoText.draw(vec2f(501, 5));
    ammoText.color = rgba(1);
    ammoText.draw(vec2f(500, 4));
}

}
