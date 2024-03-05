#include "Application.h"
#include "AppTime.h"
#include "AssetManager.h"

 Application::Application(ApplicationSpecs specs){
    //init glfw
     //m_ApplicationLayer = (ApplicationLayer*)m_LayerController.CreateLayer(new ApplicationLayer(specs));
     Core::EmptyLogFile();
     glfwInit();
     glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
     glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


     m_Window = new Window(specs.WindowWidth, specs.WindowHeight, specs.WindowTitle);
     RendererDesc desc{};
     desc.VertexCountPerDrawCall = 4*3 ;
     desc.ClearColor = specs.ClearColor;


     m_InputSystem.Init(m_Window->GetHandle());
     m_Renderer = new Renderer(desc, m_Window->GetHandle(), &m_InputSystem,&m_AssetManager);

     m_Camera.Init({ -1.5f,-1.5f }, { 3.0f,3.0f });

     m_GUIRenderer = new GUIRenderer(this);
     m_AssetManager.Init(this);
     m_AssetManager.LoadAllResources("C:\\Repos\\VulkanEngine\\Resources\\Textures\\",ResourceType::TEXTURE);

     m_Renderer->InitializePipeline(m_AssetManager.GetResourceCount(ResourceType::TEXTURE));

 }

 void Application::AddLayer(Layer* layer)
 {
     layer->SetSystem(this);
    m_LayerController.CreateLayer(layer);
 }

 Float2 Application::GetMousePos()
 {
     return { m_InputSystem.GetMousePos().x,m_Renderer->GetViewPortExtent().height-m_InputSystem.GetMousePos().y };
 }

 Float2 Application::GetMousePosNorm()
 {
     return { m_InputSystem.GetMousePos().x/m_Renderer->GetViewPortExtent().width,1.0f-(m_InputSystem.GetMousePos().y / m_Renderer->GetViewPortExtent().height )};
 }

 Float2 Application::GetWorldMousePos()
 {
     return m_InputSystem.GetWorldMousePos(m_Camera.GetPosition(), m_Camera.GetScale(), { (float)m_Renderer->GetViewPortExtent().width,(float)m_Renderer->GetViewPortExtent().height });
 }

 void Application::Run(){
    while(!glfwWindowShouldClose(m_Window->GetHandle())){
        
        m_DeltaTime = Time::GetTimeMs() - m_LastFrameTime;
        m_LastFrameTime = Time::GetTimeMs();
        m_Renderer->BeginFrame(&m_Camera);
        m_GUIRenderer->BeginGUI();

        m_LayerController.UpdateLayers(m_DeltaTime);
        m_LayerController.UpdateGUILayers();

        m_Renderer->EndFrame();

      
        m_InputSystem.ResetInput();
        glfwSwapBuffers(m_Window->GetHandle());
        glfwPollEvents();
    }
 }

 