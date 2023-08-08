#pragma once
#include "RendCore.h"
class Window{
public:
 Window(uint32_t Width,uint32_t Height,std::string name);
 GLFWwindow* GetHandle(){return m_Window;}
private:
    GLFWwindow* m_Window{};
    VkSurfaceKHR m_Surface{};
};