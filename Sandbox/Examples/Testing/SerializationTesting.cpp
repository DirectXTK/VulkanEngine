#include "SerializationTesting.h"
#include "Serializer.h"

SerializationTestingLayer::SerializationTestingLayer() : Layer("SerializationTestingLayer")
{

}
void SerializationTestingLayer::OnCreate() {

   
    SerializerFormat formats[]={
        {"Name",Format::STRING,1,"",offsetof(UnitEX,Name)},
        {"Position",Format::FLOAT,1,",",offsetof(UnitEX,Position)},
        {"Health",Format::INT32,1,"",offsetof(UnitEX,Health)},
    };

    m_Serializer.Init(formats,3);
    m_Units.push_back(UnitEX{"Kestis",Float2{0.33f,1.33f},5});

}
void SerializationTestingLayer::OnEvent(Event& event){
    KeyBoardEvent kEvent{};
    MouseEvent mEvent{};
    WindowShutDownEvent wEvent{};

    if(event.GetEventType() == EventType::KEYBOARD){
        kEvent = (KeyBoardEvent&)event;
        OnKeyBoardEvent(kEvent);

    }else if(event.GetEventType() == EventType::MOUSE){
        mEvent = (MouseEvent&)event;
        OnMouseEvent(mEvent);
    }else if(event.GetEventType() == EventType::WINDOWSHUTDOWN){
        wEvent = (WindowShutDownEvent&)event;
        Core::Log("Shuttingdown...");
        OnWindowShutDownEvent(wEvent);
    }
}
void SerializationTestingLayer::OnKeyBoardEvent(KeyBoardEvent& event){
   
}
void SerializationTestingLayer::OnWindowShutDownEvent(WindowShutDownEvent& event){

}
void SerializationTestingLayer::OnMouseEvent(MouseEvent& event){

}
void SerializationTestingLayer::OnUpdate(double deltatime)
{
   
}

void SerializationTestingLayer::OnGUI()
{   
       SerializerFormat formats[]={
        {"Name",Format::STRING,1,"",offsetof(UnitEX,Name)},
        {"Position",Format::FLOAT,2,",",offsetof(UnitEX,Position)},
        {"Health",Format::INT32,1,"",offsetof(UnitEX,Health)},
    };
    uint64_t count{};
    SerializerClassDesc classDesc{};
    classDesc.ClassName = "Unit";
    classDesc.Stride = sizeof(UnitEX);

    GUIRenderer* guiRenderer = Application::GetGUIRenderer();
    if(guiRenderer->Button("INPUT","SAVE",{-0.5f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.2f})){
        m_Serializer.StartSaving("SerializationTestingLayerSave.txt");
        m_Serializer.Save(&m_Units[0],1,formats,3,&classDesc);
    }
     if(guiRenderer->Button("LOAD","LOAD",{0.5f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.2f})){
        m_Serializer.StartLoading("SerializationTestingLayerSave.txt");
        UnitEX* units = (UnitEX*)m_Serializer.Load(&count);

        for(uint32_t i=0;i < count;i++){
            Core::Log("Name:",units[i].Name);
            Core::Log("Position:",units[i].Position.x," ",units[i].Position.y);
            Core::Log("Health:",units[i].Health);

        }
    }
	
}

void SerializationTestingLayer::OnDestroy()
{
}
void SerializationTestingLayer::OnRender(double deltime){

}
