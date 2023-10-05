#include "Application.h"
#include "Utils.h"
class SandboxLayer :public Layer {
public:
    SandboxLayer() :Layer("SandboxLayer") {}
    void OnCreate()override {

        struct Unit {
            std::string Name{};
            uint32_t Damage{};
            float Size[2]{};
        };

        Unit Units[] = {
            {"Vytas",5,{5,5}},
            {"Laurys",0,{0.66f,0.33f}}


        };
        SerializerClassDesc spec[] = {
            {"Unit",sizeof(Unit),"  ",""}
        };
        SerializerFormat formats[] = {
            {"Name=",Format::STRING,1},
            {"Damage=",Format::UINT32,1},
            {"Size=",Format::FLOAT,2,","},

        };

        Serializer serializer{};
        //serializer.Init(formats, 3);
        serializer.StartSaving("C:\\Users\\jasiu\\Desktop\\TestingSave.txt");
        for (int i = 0; i < 1; i++) {

            serializer.Save(Units,2,formats,3,spec);

        }
        serializer.StopSaving();


        //Serializer::Save("C:\\Users\\jasiu\\Desktop\\TestingSave.txt", formats[0]);
    }
    void OnUpdate()override {
        m_Renderer->DrawQuad({ 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f,1.0f }, { 1.0f,1.0f }, nullptr, 55);
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

    Application app(specs);
    app.AddLayer(new SandboxLayer());
    app.Run();
  




   
    return 1;
}