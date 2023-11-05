#define DEMO_FEATURES
#include "Testing/TestingLayer.h"
#include "Utils.h"

class SandboxLayer :public Layer {
public:
    SandboxLayer() :Layer("SandboxLayer") {}
    void OnCreate()override {

      

    }
    void OnUpdate(double deltatime)override {
    }
    void OnDestroy()override {

    }
    void OnGUI()override {
    
       
       


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