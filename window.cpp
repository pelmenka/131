#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include "render/render.h"
#include "window.h"
#include "input.h"
#include "log.h"

GLFWwindow *handle = 0;
uchar inputKeys[GLFW_KEY_LAST+1] = {0};
int lastChar, scroll;
vec2d cursorPos;
double _oldTime;

namespace window
{

double deltaTime;

void createWindow(const vec2i &size, const std::string &name)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 16);
    handle = glfwCreateWindow(size.x, size.y, name.c_str(), 0, 0);
    glfwMakeContextCurrent(handle);
    glfwSwapInterval(1);
    glewInit();
    glfwSetKeyCallback(handle, [](GLFWwindow*, int key, int, int act, int){inputKeys[key] = act;});
    glfwSetMouseButtonCallback(handle, [](GLFWwindow*, int key, int act, int){inputKeys[key] = act;});
    glfwSetWindowSizeCallback(handle, [](GLFWwindow*, int x, int y){render::viewport(vec2u(x, y));});
    glfwSetCursorPosCallback(handle, [](GLFWwindow*, double x, double y){cursorPos = vec2d(x, y);});

    _oldTime = 0;
    deltaTime = 0;

    _log::out("%s / %s / GLSL version: %s\n", glGetString(GL_VENDOR), glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void swapBuffers()
{
    deltaTime = glfwGetTime()-_oldTime;
    _oldTime = glfwGetTime();
    if(deltaTime < 0.016667)
        usleep(1667-int((deltaTime)*1000));
    glfwSwapBuffers(handle);
    lastChar = 0;
    scroll = 0;
}

void closeWindow()
{
    glfwDestroyWindow(handle);
}

bool shouldClose()
{
    return glfwWindowShouldClose(handle);
}

vec2i getSize()
{
    int w, h;
    glfwGetWindowSize(handle, &w, &h);
    return vec2i(w, h);
}

}

namespace input
{
uchar getKey(uint key)
{
    if(key > GLFW_KEY_LAST)
        return 0;
    return inputKeys[key];
}

const vec2d &getCursor()
{
    return cursorPos;
}

float getScroll()
{
    return scroll;
}

uint getSymbol()
{
    return lastChar;
}

void setKey(uint key, uchar val)
{
    if(key > GLFW_KEY_LAST)
        return;
    inputKeys[key] = val;
}

void setCursor(vec2d val)
{
    cursorPos = val;
}
}


