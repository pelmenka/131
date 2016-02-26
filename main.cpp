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

void init();
void close();
void _glfwError(int, const char*);

int main()
{
    init();
    render::effect2D test("soul.xml");
    test.start();
    render::image img("data.png");
    render::textureAtlas atlas("data/textures/particles.atlas");

    render::icon icon = atlas.getIcon(3);

    while(!window::shouldClose())
    {
        test.pos = input::getCursor();
        test.update();
        render::clearScreen();
        render::mode2D();
        //img.draw();
        test.draw();
        icon.draw(vec2f(50));
        render::endDraw();
        window::swapBuffers();
        glfwPollEvents();
    }
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