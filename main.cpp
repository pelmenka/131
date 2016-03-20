#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <stdlib.h>
#include "log.h"
#include "window.h"
#include <stdio.h>
#include "resources.h"
#include "input.h"
#include <cmath>
#include "color.h"
#include "render/render.h"
#include "render/particles.h"
#include "hero.h"
#include "level.h"
#include "render/model.h"
#include "render/text.h"
#include "hud.h"

void init();
void close();
void _glfwError(int, const char*);

int main()
{
    init();
    resources::init();
    level::init();
    hud::init();

    render::effect2D cooler("data/fx/cooler.xml");

    cooler.start();

    while(!window::shouldClose())
    {
        hud::update();
        level::update();
        cooler.update();


        render::clearScreen();
        render::mode2D();
        cooler.draw();
        level::draw();
        render::endDraw();

        render::camera.pos = vec3f(0);
        render::mode2D();
        hud::draw();
        render::endDraw();
        window::swapBuffers();
        glfwPollEvents();
    }
    level::close();

    close();
}

void init()
{
    _log::init();
    srand(time(0));
    glfwSetErrorCallback(_glfwError);
    glfwInit();

    window::createWindow(vec2i(1024, 600), "123");
    render::initRender();
    glEnable(GL_LINE_SMOOTH);
}

void close()
{
    render::closeRender();
    window::closeWindow();
    glfwTerminate();
    _log::close();
    puts("=======END=======");
}

void _glfwError(int type, const char* error)
{
    bool fatal = 0;
    switch(type)
    {
    case GLFW_NOT_INITIALIZED:
    case GLFW_NO_CURRENT_CONTEXT:
    case GLFW_OUT_OF_MEMORY:
    case GLFW_API_UNAVAILABLE:
    case GLFW_VERSION_UNAVAILABLE:
    case GLFW_PLATFORM_ERROR:
        fatal = 1;
        break;
    }
    const char *errCodes[] = {"[FATAL ERROR]", "[ERROR]"};
    _log::out("%s %s\n", errCodes[fatal], error);
    if(fatal) abort();
}
