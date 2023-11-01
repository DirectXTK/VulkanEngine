#define DEMO_FEATURES
#include "Application.h"
#include "Utils.h"
class SandboxLayer :public Layer {
public:
    SandboxLayer() :Layer("SandboxLayer") {}
    void OnCreate()override {

        struct Unit {
            std::string Name{};
            uint32_t Damage{};
            float Position[3]{};
        };

       
        Unit units[3]{};
        units[0] = { "Vyte",50,{55.777f,69.f,0.0f} };
        units[1] = { "Laurys",10,{11.777f,89.f,3.0f} };
        units[2] = { "Arthur",5,{555.777f,13.f,1.0f} };

        SerializerFormat formats[] = {
            {"Name=",Format::STRING,1,",",false},
            {"Damage=",Format::UINT32,1,",",false},
            {"Position=",Format::FLOAT,3,",",true},


        };

        SerializerClassDesc spec{};
        spec.ClassName = "Unit";
        spec.MemberPrefix = " ";
        spec.Stride = sizeof(Unit);


        Serializer serializer{};
        //serializer.Init(formats, 3);
        serializer.StartSaving("C:\\Users\\jasiu\\Desktop\\TestingSave.txt");

            serializer.Save(units, 2, formats, 3, &spec);

        serializer.StopSaving();

    }
    void OnUpdate()override {
          
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
    app.AddLayer(new SandboxLayer());
    app.Run();
  




   
    return 1;
}