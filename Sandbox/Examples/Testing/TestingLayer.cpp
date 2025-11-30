#include "TestingLayer.h"
#include "Serializer.h"

TestingLayer::TestingLayer() : Layer("TestingLayer")
{
    
}
void TestingLayer::OnCreate() {

    

}
void TestingLayer::OnUpdate(double deltatime)
{
 

}
void TestingLayer::OnEvent(Event& event){
    KeyBoardEvent& kEvent{(KeyBoardEvent&)event};
    if(event.GetEventType() == EventType::KEYBOARD){
        OnKeyBoardEvent(kEvent);
    }

}
void TestingLayer::OnKeyBoardEvent(KeyBoardEvent& event){
    if(event.State == EventState::PRESSED){
        Core::Log(GetLayerName(),":Pressed");
        if(event.Key == KeyCodes::N){
            TransitionLayer<MenuLayer>();
        }
              if(event.Key == KeyCodes::C){
            Application::AddLayer<MenuLayer>();
        }
          if(event.Key == KeyCodes::R){
            Application::RemoveLayer<MenuLayer>();
        }
        event.Expend();
    }
}
void TestingLayer::OnRender(double deltime){
    Renderer* render =Application::GetRenderer();
    render->DrawQuad({0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f,1.0f},{0.1f,0.1f},0);
}
void TestingLayer::OnGUI()
{
}

void TestingLayer::OnDestroy()
{
}

/////////////////////////////////////////////

MenuLayer::MenuLayer() : Layer("MenuLayer")
{
    
}
void MenuLayer::OnEvent(Event& event){
    KeyBoardEvent& kEvent{(KeyBoardEvent&)event};

    if(event.GetEventType() == EventType::KEYBOARD){
        OnKeyBoardEvent(kEvent);
    }
}
void MenuLayer::OnKeyBoardEvent(KeyBoardEvent& event){
    if(event.State == EventState::PRESSED){
        Core::Log(GetLayerName(),":Pressed");
        if(event.Key == KeyCodes::N){
            TransitionLayer<TestingLayer>();
        }
        if(event.Key == KeyCodes::C){
            Application::AddLayer<TestingLayer>();
        }
          if(event.Key == KeyCodes::R){
            Application::RemoveLayer<TestingLayer>();
        }
        event.Expend();
    }
}
void MenuLayer::OnRender(double deltaTime){
    Renderer* render =Application::GetRenderer();
    render->DrawQuad({0.0f,0.0f,0.0f},{0.0f,0.0f,1.0f,1.0f},{0.3f,0.05f},0);
}




