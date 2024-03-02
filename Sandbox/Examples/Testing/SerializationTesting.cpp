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
    uint32_t* Index{};
    uint32_t IndexCount{};
    void** Data{};
    Format* Formats{};
    uint32_t FormatCount{};
    GUUID TextureHandle{};
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
        m_App->m_Renderer->DrawQuad({ unit->Position.x,unit->Position.y,0.0f }, { unit->Color.r,unit->Color.g,unit->Color.b,1.0f },{ 0.08f,0.08f },unit->TextureHandle, unit->ID);
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
    if (m_App->m_GUIRenderer->Button({ -0.5f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.1f,0.1f }, MouseCodes::LEFT, false)) {
        m_Units.push_back(Unit());
        MakeUnitRandom(&m_Units[m_Units.size() - 1]);
    }
}

void SerializationTestingLayer::OnDestroy()
{
}

void SerializationTestingLayer::AllocSpaceAndFillData(void** DataPtr,Format format,uint32_t Index)
{
    switch (format) {
    case Format::STRING: {
        DataPtr[Index] = new std::string();
        *(std::string*)DataPtr[Index] = "LafaTafa";
        break;
    }
    case Format::UINT16: {
        DataPtr[Index] = new uint16_t();
        *(uint16_t*)DataPtr[Index] = 16;

        break;
    }
    case Format::UINT32: {
        DataPtr[Index] = new uint32_t();
        *(uint32_t*)DataPtr[Index] = 32;

        break;
    }
    case Format::UINT64: {
        DataPtr[Index] = new uint64_t();
        *(uint64_t*)DataPtr[Index] = 64;

        break;
    }
    case Format::INT16: {
        DataPtr[Index] = new int16_t();
        *(int16_t*)DataPtr[Index] = -16;

        break;
    }
    case Format::INT32: {
        DataPtr[Index] = new uint32_t();
        *(uint32_t*)DataPtr[Index] = -32;

        break;
    }
    case Format::INT64: {
        DataPtr[Index] = new uint64_t();
        *(uint64_t*)DataPtr[Index] = -64;

        break;
    }
    case Format::FLOAT: {
        DataPtr[Index] = new float();
        *(float*)DataPtr[Index] = 64.4;

        break;
    }
    case Format::DOUBLE: {
        DataPtr[Index] = new double();
        *(double*)DataPtr[Index] = 64.44;

        break;
    }
    case Format::CHAR: {
        DataPtr[Index] = new char();
        *(char*)DataPtr[Index] = 'C';

        break;
    }


    default: {
        std::cout << "No such format\n";
        return;
    }
    }
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
    {"Index=",Format::UINT32,-1,",",offsetof(Unit,Index) },
    {"IndexCount=",Format::UINT32,1,",",offsetof(Unit,IndexCount) },
    {"Data=",Format::FORMAT,-1,",",offsetof(Unit,Data) },
    {"Formats=",Format::UINT32,-1,",",offsetof(Unit,Formats) },
    {"FormatCount=",Format::UINT32,1,",",offsetof(Unit,FormatCount) },

    };

    SerializerClassDesc desc{ "Unit",sizeof(Unit)," ","" };
    Serializer serializer;
    serializer.StartSaving("C:\\Users\\jasiu\\Desktop\\TestingSave.txt");
    serializer.Save(m_Units.data(), m_Units.size(), format, ARRAYSIZE(format), &desc);
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

void SerializationTestingLayer::MakeUnitRandom(Unit* unit)
{
    unit->Position = { Core::RandomFloat(-1.0,1.0f),Core::RandomFloat(-1.0,1.0f) };
    unit->Color = { Core::RandomFloat(0.0,1.0f),Core::RandomFloat(0.0,1.0f),Core::RandomFloat(0.0,1.0f) };
    unit->Name = "MAKINGTHESTRINGLONGSOOTODISABLETHESMALLSTRINGOPTIMIZATION";
    uint32_t TraitCount = Core::RandomUInt32(0, 8);
    unit->TraitCount = TraitCount;
    unit->IndexCount = 5;
    unit->Index = new uint32_t[unit->IndexCount];

    unit->FormatCount = Core::RandomUInt32(0, 8);
    unit->Formats = new Format[unit->FormatCount];
    unit->Data = new void* [unit->FormatCount];

    GUUID lafa = std::hash<std::string>{}("C:\\Repos\\VulkanEngine\\Resources\\Textures\\Texture1.png");
    GUUID lafa2 = std::hash<std::string>{}("C:\\Repos\\VulkanEngine\\Resources\\Textures\\Texture2.png");
    GUUID lafa3 = std::hash<std::string>{}("C:\\Repos\\VulkanEngine\\Resources\\Textures\\Texture3.png");


    GUUID ids[3];
    ids[0] = lafa;
    ids[1] = lafa2;
    ids[2] = lafa3;

    unit->TextureHandle = ids[Core::RandomUInt64(0,2)];
   


    for (uint32_t i = 0; i < unit->FormatCount; i++) {

    unit->Formats[i] = (Format)Core::RandomUInt32(1,10);
    AllocSpaceAndFillData(unit->Data, unit->Formats[i], i);

    }


    
   





    for (uint32_t i = 0; i < unit->IndexCount; i++) {
        unit->Index[i] = i * 5;
    }
    if (TraitCount == 0)
        return;
    unit->Traits = new std::string[TraitCount];
    for (int i = 0; i < TraitCount; i++) {
        unit->Traits[i] = std::string();
        unit->Traits[i] = TRAITS[Core::RandomUInt32(0, 8)];
        unit->ID = Core::RandomUInt64(0, (std::numeric_limits<uint64_t>::max)());
    }
    

}
