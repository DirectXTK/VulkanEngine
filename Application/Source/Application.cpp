#include "Application.h"
#include "AppTime.h"
 Application::Application(ApplicationSpecs specs){
    //init glfw
     m_ApplicationLayer = (ApplicationLayer*)m_LayerController.CreateLayer(new ApplicationLayer(specs));
     m_Renderer = m_ApplicationLayer->GetRenderer();
     m_Camera = m_ApplicationLayer->GetCamera();
 }

 void Application::AddLayer(Layer* layer)
 {
     layer->SetSystem(m_Camera, m_Renderer);
    m_LayerController.CreateLayer(layer);
 }

 void Application::Run(){
     m_Window = m_ApplicationLayer->GetWindow();
    while(!glfwWindowShouldClose(m_Window->GetHandle())){
        
        m_DeltaTime = Time::GetTimeMs() - m_LastFrameTime;
        m_LastFrameTime = Time::GetTimeMs();
        m_Renderer->BeginFrame(m_Camera->GetViewProj());

        m_LayerController.UpdateLayers();

        m_Renderer->EndFrame();

    }
 }

 ApplicationLayer::ApplicationLayer(ApplicationSpecs specs) : Layer("ApplicationLayer")
 {
     m_Specs = specs;
 }
 void ApplicationLayer::OnCreate() {
      Core::EmptyLogFile();
      glfwInit();
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


      m_Window = new Window(m_Specs.WindowWidth, m_Specs.WindowHeight, m_Specs.WindowTitle);
      RendererDesc desc{};
      desc.VertexCountPerDrawCall = 4 * 10000;
      desc.ClearColor = m_Specs.ClearColor;


      m_Renderer = new Renderer(desc, m_Window->GetHandle());
      m_InputSystem.Init(m_Window->GetHandle());

      m_Camera.Init({ -1.5f,-1.5f }, { 3.0f,3.0f });

      Image image("Map2.png");
 }

  void ApplicationLayer::OnUpdate()
  {
    


      // for(int i =0;i < 26;i++){
     
      // }
                //m_Renderer->DrawQuad({0.42f,0.0f,0.0f},{0.0f,1.0f,1.0f,0.0f},{0.2f,0.2f});

      if (m_InputSystem.IsKeyPressed(KeyCodes::D)) {
          Float2 pos = m_Camera.GetPosition();
          pos.x += m_Direct;
          m_Camera.SetPosition(pos);
      }
      else if (m_InputSystem.IsKeyPressed(KeyCodes::A)) {
          Float2 pos = m_Camera.GetPosition();
          pos.x -= m_Direct;
          m_Camera.SetPosition(pos);
      }
      else if (m_InputSystem.IsKeyPressed(KeyCodes::W)) {
          Float2 pos = m_Camera.GetPosition();
          pos.y += m_Direct;
          m_Camera.SetPosition(pos);
      }
      else if (m_InputSystem.IsKeyPressed(KeyCodes::S)) {
          Float2 pos = m_Camera.GetPosition();
          pos.y -= m_Direct;
          m_Camera.SetPosition(pos);
      }

      if (m_InputSystem.IsKeyPressed(KeyCodes::X)) {
          Float2 size = m_Camera.GetScale();
          size.x += 0.1f;
          size.y += 0.1f;

          m_Camera.SetScale(size);
      }
      if (m_InputSystem.IsKeyPressed(KeyCodes::Z)) {
          Float2 size = m_Camera.GetScale();
          size.x -= 0.1f;
          size.y -= 0.1f;

          m_Camera.SetScale(size);
      }
      if (m_InputSystem.GetScroll() != 0) {
          Float2 size = m_Camera.GetScale();
          size.x -= 0.1f * m_InputSystem.GetScroll();
          size.y -= 0.1f * m_InputSystem.GetScroll();

          m_Camera.SetScale(size);
      }

      if (m_InputSystem.IsMouseClicked(MouseCodes::LEFT)) {
          Buffer* buffer = m_Renderer->GetCustomBuffer(0);
          //  Core::Log(ErrorType::Info,"Pos", m_InputSystem.GetMousePos().x, " ", m_InputSystem.GetMousePos().y);

          Float2 a = buffer->ReadPixel((uint32_t)m_InputSystem.GetMousePos().x, (uint32_t)m_InputSystem.GetMousePos().y, m_Renderer->GetViewPortExtent().width, m_Renderer->GetViewPortExtent().height);
          uint64_t* ID = (uint64_t*)&a;
          Core::Log(ErrorType::Info, "ID ", *ID);
      }




      // Core::Log( ErrorType::Info,m_InputSystem.GetScroll());

      // m_Renderer->Statistics();

      m_InputSystem.ResetInput();
      glfwSwapBuffers(m_Window->GetHandle());
      glfwPollEvents();
  }

  void ApplicationLayer::OnGUI()
  {
  }

  void ApplicationLayer::OnDestroy()
  {
  }



