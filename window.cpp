#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include "render/render.h"
#include "window.h"
#include "input.h"

GLFWwindow *handle = 0;
uchar inputKeys[GLFW_KEY_LAST+1] = {0};
int lastChar, scroll;
vec2d cursorPos;

namespace window
{

void createWindow(const vec2i &size, const std::string &name)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    handle = glfwCreateWindow(size.x, size.y, name.c_str(), 0, 0);
    glfwMakeContextCurrent(handle);
    glfwSwapInterval(1);
    glewInit();
    glfwSetKeyCallback(handle, [](GLFWwindow*, int key, int, int act, int){inputKeys[key] = act;});
    glfwSetWindowSizeCallback(handle, [](GLFWwindow*, int x, int y){render::viewport(vec2u(x, y));});
    glfwSetCursorPosCallback(handle, [](GLFWwindow*, double x, double y){cursorPos = vec2d(x, y);});
}

void swapBuffers()
{
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


