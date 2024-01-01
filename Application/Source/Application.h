#pragma once
#include "AppCore.h"
#include "Renderer.h"
#include "Window.h"
#include "InputSystem.h"
#include "Layer.h"
class InputSystem;
struct ApplicationSpecs{
    uint32_t WindowHeight{500};
     uint32_t WindowWidth{500};
    std::string WindowTitle{"Untitled"};

    //Renderer stuff
    Float4 ClearColor{ 1.0f,1.0f,1.0f,1.0f };
};


class Application {
public:
    Application(ApplicationSpecs specs);
    void AddLayer(Layer* layer);
    float GetDeltaTime() { return m_DeltaTime; }

    Float2 GetMousePos();
    Float2 GetMousePosNorm();
    Float2 GetWorldMousePos();



    void Run();
private:
    Window* m_Window{};

    LayerController m_LayerController{ };

    double m_DeltaTime{};
    double m_LastFrameTime{};
   
public:
    //GUI Renderer
    Renderer* m_Renderer{};
    Camera2D m_Camera{};
    InputSystem m_InputSystem{};


#ifdef GUI_HEADER
    GUIRenderer* m_GUIRenderer{};
#endif

};