#define DEMO_FEATURES
//#include "..//Examples/Testing/GUITesting.h"
//#include "..//Examples/Testing/SerializationTesting.h"
//#include "..//Examples/Testing/TestingLayer.h"
#//include "..//Examples/Testing/DLLLoadingLayer.h"
//#include "../Examples/Testing/TextureTestingLayer.h"
#include "../Examples/Testing/AnimationTestingLayer.h"

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
    specs.ClearColor = { 0.5f,0.5,0.5f,0.0f };
    specs.WindowTitle = "OPA";
    Application app(specs);
    app.AddLayer(new AnimationTestingLayer());
    app.Run();

   
    return 1;
}