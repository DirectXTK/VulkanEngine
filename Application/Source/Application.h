#pragma once
#include "AppCore.h"
#include "Renderer.h"
#include "Window.h"
#include "InputSystem.h"
#include "Layer.h"
struct ApplicationSpecs{
    uint32_t WindowHeight{500};
     uint32_t WindowWidth{500};
    std::string WindowTitle{"Untitled"};
};

class ApplicationLayer:public Layer {
public:
    ApplicationLayer( ApplicationSpecs spces);
     void OnCreate()override;
     void OnUpdate()override;
     void OnGUI()override;
     void OnDestroy()override;

     //Get functions
     Window* GetWindow() { return m_Window; }
     Renderer* GetRenderer() { return m_Renderer; }
     Camera2D* GetCamera() { return &m_Camera; }
private:
    Renderer* m_Renderer{};
    Window* m_Window{};
    InputSystem m_InputSystem{};
    Camera2D m_Camera;
    ApplicationSpecs m_Specs{  };


    //Controlls
    float m_Move{ -1.0 };
    float m_Direct{ 0.005f };
};
class Application {
public:
    Application(ApplicationSpecs specs);
    void AddLayer(Layer* layer);
    void Run();
private:
    Renderer* m_Renderer{};
    Window* m_Window{};
    InputSystem m_InputSystem{};
    Camera2D* m_Camera{};

    LayerController m_LayerController{ };
    ApplicationLayer* m_ApplicationLayer{ };
};