#include "SerializationTesting.h"
#include "Serializer.h"
std::string TRAITS[] = {
    "Strong",
    "Weak",
    "Wise",
    "Rich",
    "Poor",
    "Big",
    "Small",
    "Noble",
    "Brave",

};
struct Unit {
    Float3 Color{};
    Float2 Position{};
    std::string* Traits{};
    uint32_t TraitCount{};
    uint64_t ID{};
    std::string Name{};
};
SerializationTestingLayer::SerializationTestingLayer() : Layer("SerializationTestingLayer")
{

}
void SerializationTestingLayer::OnCreate() {

   


}
void SerializationTestingLayer::OnUpdate(double deltatime)
{
    Camera2D* camera = &m_App->m_Camera;
    InputSystem* inputsystem = &m_App->m_InputSystem;
    Renderer* renderer = m_App->m_Renderer;

    // for(int i =0;i < 26;i++){

    // }
              //m_Renderer->DrawQuad({0.42f,0.0f,0.0f},{0.0f,1.0f,1.0f,0.0f},{0.2f,0.2f});

    if (inputsystem->IsKeyPressed(KeyCodes::D)) {
        Float2 pos = camera->GetPosition();
        pos.x += m_Direct;
        camera->SetPosition(pos);
    }
    else if (inputsystem->IsKeyPressed(KeyCodes::A)) {
        Float2 pos = camera->GetPosition();
        pos.x -= m_Direct;
        camera->SetPosition(pos);
    }
    else if (inputsystem->IsKeyPressed(KeyCodes::W)) {
        Float2 pos = camera->GetPosition();
        pos.y += m_Direct;
        camera->SetPosition(pos);
    }
    else if (inputsystem->IsKeyPressed(KeyCodes::S)) {
        Float2 pos = camera->GetPosition();
        pos.y -= m_Direct;
        camera->SetPosition(pos);
    }

    if (inputsystem->IsKeyPressed(KeyCodes::X)) {
        Float2 size = camera->GetScale();
        size.x += 0.1f;
        size.y += 0.1f;

        camera->SetScale(size);
    }
    if (inputsystem->IsKeyPressed(KeyCodes::Z)) {
        Float2 size = camera->GetScale();
        size.x -= 0.1f;
        size.y -= 0.1f;

        camera->SetScale(size);
    }
    if (inputsystem->GetScroll() != 0) {
        Float2 size = camera->GetScale();
        size.x -= 0.1f * inputsystem->GetScroll();
        size.y -= 0.1f * inputsystem->GetScroll();

        camera->SetScale(size);
    }

    if (inputsystem->IsMouseClicked(MouseCodes::LEFT)) {
        Buffer* buffer = renderer->GetCustomBuffer(0);
        //  Core::Log(ErrorType::Info,"Pos", inputsystem->GetMousePos().x, " ", inputsystem->GetMousePos().y);

        Float2 a = buffer->ReadPixel((uint32_t)inputsystem->GetMousePos().x, (uint32_t)inputsystem->GetMousePos().y, renderer->GetViewPortExtent().width, renderer->GetViewPortExtent().height);
        uint64_t* ID = (uint64_t*)&a;
        if (*ID != 0) {
            for (int i = 0; i < m_Units.size(); i++) {
                if (*ID == m_Units[i].ID)
                {
                    std::cout <<"TraitCount " << m_Units[i].TraitCount<<"\n";
                    std::cout << "Traits ";
                    for (int j = 0; j < m_Units[i].TraitCount; j++) {
                        if (j + 1 < m_Units[i].TraitCount)
                            std::cout << ",";
                        std::cout << m_Units[i].Traits[j];
                    }
                    std::cout << "\n";
                        break;
                }
            }
        }
        // Core::Log(ErrorType::Info, "ID ", *ID);
    }

    for (uint32_t i = 0; i < m_Units.size(); i++) {
        Unit* unit = &m_Units[i];
        m_App->m_Renderer->DrawQuad({ unit->Position.x,unit->Position.y,0.0f }, { unit->Color.r,unit->Color.g,unit->Color.b,1.0f }, { 0.08f,0.08f }, nullptr,unit->ID);
    }



    // Core::Log( ErrorType::Info,inputsystem->GetScroll());

    // m_Renderer->Statistics();


}

void SerializationTestingLayer::OnGUI()
{
	if (m_App->m_GUIRenderer->Button({ 0.0f,0.0f }, { 1.0f,1.0f,0.0f,1.0f }, { 0.1f,0.1f },MouseCodes::LEFT,false)) {
        Save();
	}
    if (m_App->m_GUIRenderer->Button({ 0.5f,0.0f }, { 1.0f,0.0f,0.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, false)) {
        Load();
    }
    if (m_App->m_GUIRenderer->Button({ -0.5f,0.0f }, { 0.0f,0.0f,1.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, false)) {
        m_Units.push_back(Unit());
        MakeUnitRandom(&m_Units[m_Units.size() - 1]);
    }
}

void SerializationTestingLayer::OnDestroy()
{
}

void SerializationTestingLayer::Save()
{
   

    SerializerFormat format[] = {
    {"Color=",Format::FLOAT,3,",",offsetof(Unit,Color) },
    {"Position=",Format::FLOAT,2,",",offsetof(Unit,Position) },
    {"Traits=",Format::STRING,-1,",",offsetof(Unit,Traits) },
    {"TraitCount=",Format::UINT32,1,",",offsetof(Unit,TraitCount) },
    {"GUUID=",Format::UINT64,1,",",offsetof(Unit,ID) },
    {"Name=",Format::STRING,1,",",offsetof(Unit,Name) },

    };

    SerializerClassDesc desc{ "Unit",sizeof(Unit)," ","" };
    Serializer serializer;
    serializer.StartSaving("C:\\Users\\jasiu\\Desktop\\TestingSave.txt");
    serializer.Save(m_Units.data(), m_Units.size(), format, 6, &desc);
    serializer.StopSaving();
}

void SerializationTestingLayer::Load()
{
    uint64_t NumOfUnits{};
    void* Data{};
    Unit* units{};

    Serializer serializer;
    serializer.StartLoading("C:\\Users\\jasiu\\Desktop\\TestingSave.txt");

    Data =  serializer.Load(&NumOfUnits);
    serializer.StopLoading();

    units = (Unit*)Data;
    m_Units.resize(NumOfUnits);
    for (uint32_t i = 0; i < NumOfUnits; i++) {
        m_Units[i] = units[i];
    }
}

void MakeUnitRandom(Unit* unit)
{
    unit->Position = { Core::RandomFloat(-1.0,1.0f),Core::RandomFloat(-1.0,1.0f) };
    unit->Color = { Core::RandomFloat(0.0,1.0f),Core::RandomFloat(0.0,1.0f),Core::RandomFloat(0.0,1.0f) };
    unit->Name = "MAKINGTHESTRINGLONGSOOTODISABLETHESMALLSTRINGOPTIMIZATION";
    uint32_t TraitCount = Core::RandomUInt32(0, 8);
    unit->TraitCount = TraitCount;
    if (TraitCount == 0)
        return;
    unit->Traits = new std::string[TraitCount];
    for (int i = 0; i < TraitCount; i++) {
        unit->Traits[i] = std::string();
        unit->Traits[i] = TRAITS[Core::RandomUInt32(0, 8)];
        unit->ID = Core::RandomUInt64(0, std::numeric_limits<uint64_t>::max());
    }
    

}
