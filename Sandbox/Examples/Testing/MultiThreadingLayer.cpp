#include "MultiThreadingLayer.h"
#include "DefaultCameraControlls.h"


    MultiThreadingLayer::MultiThreadingLayer():Layer("MultiThreadingLayer"){

    }
    void MultiThreadingLayer::OnCreate(){
        Application::LoadAllAssets("EngineResources/Examples/Animation/",AssetType::ANIMATION);
        if(!Application::HasAsset(Core::GetStringHash("Examples/Animation/TOWN_HALL"))){
            Core::Log("Doesn't have asset");
        }

        Asset<Animator> animation =Application::GetAsset<Animator>("TOWN_HALL");
        if(animation&& animation.GetType() == AssetType::ANIMATION){

          //  m_Animation.push_back(*animation.GetData());
         //   m_Animation[0].SetStage("IDLE");
        } else{
            return;
        }
        Application::GetRender()->SetFont(Asset<Font>(),16);
        
        m_Textures.push_back(Core::GetStringHash("Examples/Animation/TOWN_HALL"));
        m_OBJ.push_back({0.0f,0.0f});

    }
    void MultiThreadingLayer::OnUpdate(double deltaTime){
          // m_Animation[0].Update(deltaTime);
          
        
    }
    void MultiThreadingLayer::OnRender(double delaTime){
        Render* render= Application::GetRender();
        
        for(uint32_t i=0 ;i < m_OBJ.size();i++){

           //render->DrawQuad({m_OBJ[i].x,m_OBJ[i].y,0.0f},{1.0f,1.0f,0.0f,1.0f},{0.2f,0.2f},0);
        }
    }
    void MultiThreadingLayer::OnDestroy(){


    }
    void MultiThreadingLayer::OnGUI(){
        //here
        GUIRenderer* gui = Application::GetGUIRenderer();
        static float fontSize{10};

        Application::GetRender()->SetFont(Asset<Font>(),13);

        Application::GetApplication()->m_FontSystem->Text("L","HOLETTHEDOGSOUTHOOF   .",{0.0,0.0},{0.3f,0.1f});
        gui->Slider("s5lider",&fontSize,{0.0f,-0.5f},{1.0f,1.0f,1.0f,1.0f},{0.1f,0.1f},0.1f,{10,16},1);
        Application::GetRender()->SetFont(Asset<Font>(),fontSize);
        Application::GetApplication()->m_FontSystem->Text("L","HOLETTHEDOGSOUTHOOF   .",{0.0,0.5},{0.3f,0.2f});
    }
    void MultiThreadingLayer::OnEvent(Event& event){
        if(event.GetEventType() == EventType::MOUSE)
            OnMouseEvent((MouseEvent&)event);
        else if(event.GetEventType() == EventType::KEYBOARD)
           OnKeyboardEvent((KeyBoardEvent&)event);

    }
    void MultiThreadingLayer::OnMouseEvent(MouseEvent& event){
        if(event.Code == MouseCodes::LEFT && event.State == EventState::PRESSED){
            return;
            Render* render = Application::GetRender();
           ParticleProps props{};
           props.Pos = Application::GetWorldMousePos();
           props.Color = {1.0f,1.0f,1.0f,1.0f};
           props.Size = {0.2f,0.2f};
           props.Alive = true;
           props.LifeTime = SEC(30.0f);
           props.TextureID = Core::GetStringHash("Particles/Fire");

           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
        }
          if(event.Code == MouseCodes::RIGHT && event.State == EventState::PRESSED){

               Render* render = Application::GetRender();
           ParticleProps props{};
           props.Pos = Application::GetWorldMousePos();
           props.Color = {1.0f,1.0f,1.0f,1.0f};
           props.Size = {0.2f,0.2f};
           props.Alive = true;
           props.LifeTime = SEC(30.0f);
           props.Animation = *Application::GetAsset<Animator>("TOWN_HALL").GetData();

           //Working on instanced rendering!!!
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
           Application::GetParticleSystem().DrawParticle(props);
        }
    }
    void MultiThreadingLayer::OnKeyboardEvent(KeyBoardEvent& event){
        Render* render = Application::GetRender();
        if(event.Key == KeyCodes::D && event.State == EventState::PRESSED){
            render->SetShader(Application::GetAsset<Shader>("Shaders/WireFrameG"));
        }
        if(event.Key == KeyCodes::O && event.State == EventState::PRESSED){
            render->RemoveShader(ShaderType::Geometry);
        }
        DefaultCameraControlls(Application::GetCurrentCamera());
    }




