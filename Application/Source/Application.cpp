#include "Application.h"
 Application::Application(ApplicationSpecs specs){
    //init glfw
   Core::EmptyLogFile();
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

 
    m_Window = new Window(specs.WindowWidth,specs.WindowHeight,specs.WindowTitle);
      RendererDesc desc{};
      desc.VertexCountPerDrawCall = 10000;

 
    m_Renderer= new Renderer(desc,m_Window->GetHandle());
    m_InputSystem.Init(m_Window->GetHandle());
 }

 void Application::Run(){
      float move{-1.0};
      float Direct{0.005f};
      Camera2D camera;
      camera.Init({-1.5f,-1.5f},{3.0f,3.0f});
  
      
    while(!glfwWindowShouldClose(m_Window->GetHandle())){
   m_Renderer->BeginFrame(camera.GetViewProj());

     // for(int i =0;i < 26;i++){
             for(int j=0;j < 250;j++){
          m_Renderer->DrawQuad({j*0.42f,0.0f,0.0f},{0.0f,1.0f,1.0f,0.0f},{0.2f,0.2f},nullptr, 18446744073709551615);

            }
     // }
               //m_Renderer->DrawQuad({0.42f,0.0f,0.0f},{0.0f,1.0f,1.0f,0.0f},{0.2f,0.2f});

        if(m_InputSystem.IsKeyPressed(KeyCodes::D)){
            Float2 pos= camera.GetPosition();
            pos.x += Direct;
            camera.SetPosition(pos);
        }
        else if(m_InputSystem.IsKeyPressed(KeyCodes::A)){
            Float2 pos= camera.GetPosition();
            pos.x -= Direct;
            camera.SetPosition(pos);
        }
        else if(m_InputSystem.IsKeyPressed(KeyCodes::W)){
            Float2 pos= camera.GetPosition();
            pos.y+= Direct;
            camera.SetPosition(pos);
        }
        else if(m_InputSystem.IsKeyPressed(KeyCodes::S)){
            Float2 pos= camera.GetPosition();
            pos.y -= Direct;
            camera.SetPosition(pos);
        }
       
           if(m_InputSystem.IsKeyPressed(KeyCodes::X)){
            Float2 size= camera.GetScale();
            size.x +=0.1f;
            size.y +=0.1f;

            camera.SetScale(size);
        }
           if(m_InputSystem.IsKeyPressed(KeyCodes::Z)){
            Float2 size= camera.GetScale();
            size.x -=0.1f;
            size.y -=0.1f;

            camera.SetScale(size);
        }
        if(m_InputSystem.GetScroll()!=0){
             Float2 size= camera.GetScale();
            size.x -=0.1f*m_InputSystem.GetScroll();
            size.y -=0.1f*m_InputSystem.GetScroll();
          
            camera.SetScale(size);
        }

        if (m_InputSystem.IsMouseClicked(MouseCodes::LEFT) ){
            Buffer* buffer =m_Renderer->GetCustomBuffer(0);
          //  Core::Log(ErrorType::Info,"Pos", m_InputSystem.GetMousePos().x, " ", m_InputSystem.GetMousePos().y);

            Float2 a = buffer->ReadPixel(m_InputSystem.GetMousePos().x, m_InputSystem.GetMousePos().y, 500, 500);
            uint64_t* ID = (uint64_t*)&a;
            Core::Log(ErrorType::Info, "ID ", *ID);
        }




           // Core::Log( ErrorType::Info,m_InputSystem.GetScroll());

      m_Renderer->EndFrame();
     // m_Renderer->Statistics();

      m_InputSystem.ResetInput();
      glfwSwapBuffers(m_Window->GetHandle());
        glfwPollEvents();

    }
 }


