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

        m_MapImage.LoadTextureData("Testing5.png",&m_Width,&m_Height);
        //Serializer::Save("C:\\Users\\jasiu\\Desktop\\TestingSave.txt", formats[0]);
    }
    void OnUpdate()override {
            for (int y = 0; y < m_MapImage.GetExtent().height; y++) {

        for (int x = 0; x < m_MapImage.GetExtent().width; x++) {

            Float4 Color{0.0f,0.0f,0.0f,1.0f};
            Color = m_MapImage.ReadPixel(x, y);

           // Core::Log(ErrorType::Info, "Texture Color ", m_Map[4]/255.f, " ", (uint16_t)m_Map[5], " ", (float)((uint32_t)m_Map[6] / 255.f), " ", (uint64_t)m_Map[7] / 255.f);
              m_Renderer->DrawQuad({ x*0.1f,(m_MapImage.GetExtent().height -y)*0.1f,0.0f }, Color, {0.05f,0.05f}, nullptr, 55);
            }
        }
    }
    void OnDestroy()override {

    }
    void OnGUI()override {

    }
private:
    Image m_MapImage{ };
    unsigned char* m_Map{};
    int m_Width{}, m_Height{};
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