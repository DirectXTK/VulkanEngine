#define DEMO_FEATURES
#include "Testing/TestingLayer.h"
#include "Utils.h"

class SandboxLayer :public Layer {
public:
    SandboxLayer() :Layer("SandboxLayer") {}
    void OnCreate()override {

      

    }
    void OnUpdate(double deltatime)override {
        //m_App->m_Renderer->DrawQuad({ 0.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f,1.0f }, { 0.10f,0.1f }, nullptr, 6);
    }
    void OnDestroy()override {

    }
    void OnGUI()override {
    
        bool* CloseButton{};

        if (m_App->m_GUIRenderer->Button({ 0.5f,0.0f }, { 0.0f,1.0f,0.0f,1.0f }, { 0.2f,0.2f }, MouseCodes::RIGHT,true, &CloseButton)) {
            if(m_App->m_GUIRenderer->Button({ 0.5f,0.0f }, { 0.0f,0.5f,0.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT,false)) {
                std::cout << "Pirmas\n";
                *CloseButton = false;
            }
           if( m_App->m_GUIRenderer->Button({ 0.5f,-0.21f }, { 0.0f,0.5f,0.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, false) ){
                std::cout << "Antras\n";
                *CloseButton = false;

            }

        }

       


    }
private:
};
int main() {
    ApplicationSpecs specs{};
    specs.WindowWidth = 500;
    specs.WindowHeight = 500;
    specs.WindowTitle = "Vulkan";
    specs.ClearColor = { 0.5f,0.5f,0.5f,1.0f };

    Application app(specs);
    app.AddLayer(new TestingLayer());
    app.AddLayer(new SandboxLayer());
    app.Run();
  




   
    return 1;
}