#include "MultiThreadingLayer.h"
#include "DefaultCameraControlls.h"
    MultiThreadingLayer::MultiThreadingLayer():Layer("MultiThreadingLayer"){

    }
    void MultiThreadingLayer::OnCreate(){

    }
    void MultiThreadingLayer::OnUpdate(double deltaTime){
    }
    void MultiThreadingLayer::OnRender(double delaTime){
        Render* render= Application::GetRender();
        for(uint32_t i=0 ;i < m_OBJ.size();i++){

          //  render->DrawQuad({m_OBJ[i].x,m_OBJ[i].y,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.02f,0.02f},0);
        }
    }
    void MultiThreadingLayer::OnDestroy(){


    }
    void MultiThreadingLayer::OnGUI(){

    }
    void MultiThreadingLayer::OnEvent(Event& event){
        if(event.GetEventType() == EventType::MOUSE)
            OnMouseEvent((MouseEvent&)event);
        else if(event.GetEventType() == EventType::KEYBOARD)
            OnKeyboardEvent((KeyBoardEvent&)event);

    }
    void MultiThreadingLayer::OnMouseEvent(MouseEvent& event){
        if(event.Code == MouseCodes::LEFT && event.State == EventState::PRESSED){
           // m_OBJ.push_back({Application::GetWorldMousePos()});
           ParticleProps props{};
           props.Pos = Application::GetWorldMousePos();
           props.Color = {1.0f,1.0f,1.0f,1.0f};
           props.Size = {0.2f,0.2f};
           props.Alive = true;
           props.LifeTime = SEC(30.0f);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);

        }
    }
    void MultiThreadingLayer::OnKeyboardEvent(KeyBoardEvent& event){
        DefaultCameraControlls(Application::GetCurrentCamera());
    }




