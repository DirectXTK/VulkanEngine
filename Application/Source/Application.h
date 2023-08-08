#pragma once
#include "AppCore.h"
#include "Renderer.h"
#include "Window.h"
#include "InputSystem.h"
struct ApplicationSpecs{
    uint32_t WindowHeight{500};
     uint32_t WindowWidth{500};
    std::string WindowTitle{"Untitled"};
};
class Application{
public:
 Application(ApplicationSpecs specs);
 void Run();
private:
 Renderer* m_Renderer{};
 Window* m_Window{};
 InputSystem m_InputSystem{};
};