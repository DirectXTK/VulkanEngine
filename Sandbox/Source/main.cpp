#define DEMO_FEATURES
#include "Application.h"
#include "Utils.h"
struct Unit {
    std::string Name{};
    Float3 Position{};
    Float2 Size{};
    Float4 Color{};
    uint64_t ID{};
};
class SandboxLayer :public Layer {
public:
    SandboxLayer() :Layer("SandboxLayer") {}
    void OnCreate()override {

      

        for (int i = 0; i < 100; i++) {
            Unit unit{};
            unit.Position = { Core::RandomFloat(-1.0f,1.0f),Core::RandomFloat(-1.0f,1.0f),0.0f };
            unit.Size = { Core::RandomFloat(0.0f,0.2f),Core::RandomFloat(0.0f,0.2f) };
            unit.Color = { Core::RandomFloat(0.0f,1.0f), Core::RandomFloat(0.0f,1.0f), Core::RandomFloat(0.0f,1.0f),1.0f };
            unit.ID = Core::RandomUInt64(0,std::numeric_limits<uint64_t>::max());
            unit.Name = "Ting galvot";
          // m_Units.push_back(unit);
        }
        SerializerClassDesc desc{};
        desc.ClassName = "Unit";
        desc.MemberPrefix = " ";
        desc.MemberSuffix = "";
        desc.Stride = sizeof(Unit);

        SerializerFormat formats[] = {
            {"Name=",Format::STRING,1,"",offsetof(Unit,Name)},
            {"Position=",Format::FLOAT,3,",",offsetof(Unit,Position)},
            {"Size=",Format::FLOAT,2,",",offsetof(Unit,Size)},
            {"Color=",Format::FLOAT,4,",",offsetof(Unit,Color)},
            {"ID=",Format::UINT64,1,"",offsetof(Unit,ID)},

        };

       
       
        uint64_t DataCount{ };
        Serializer serializer{};
       //serializer.Init(formats, 3);
    //    serializer.StartSaving("C:\\Users\\jasiu\\Desktop\\TestingSave.txt");

      //  serializer.Save(m_Units.data(), m_Units.size(), formats, 5, &desc);

    //   serializer.StopSaving();

       serializer.StartLoading("C:\\Users\\jasiu\\Desktop\\TestingSave.txt");

      Unit* data = (Unit*)serializer.Load(&DataCount);

       serializer.StopLoading();
       m_Units.resize(DataCount);
        for (int i = 0; i < DataCount; i++) {
            m_Units[i]=Unit();
           m_Units[i].Name = data[i].Name;
           m_Units[i].Position = data[i].Position;
          m_Units[i].Size = data[i].Size;
            m_Units[i].Color = data[i].Color;
            m_Units[i].ID = data[i].ID;


        }
        

    }
    void OnUpdate()override {
        for (int i = 0; i < m_Units.size(); i++) {
            m_Renderer->DrawQuad(m_Units[i].Position, m_Units[i].Color, m_Units[i].Size, nullptr, m_Units[i].ID);
          }
    }
    void OnDestroy()override {

    }
    void OnGUI()override {

    }
private:
    std::vector<Unit> m_Units{};
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