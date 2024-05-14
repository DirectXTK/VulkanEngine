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
     m_Camera.Init({ -1.5f,-1.5f }, { 1.0f,1.0f });

     RendererDesc desc{};
     desc.VertexCountPerDrawCall = 4*100 ;
     desc.ClearColor = specs.ClearColor;
     desc.InitialCamera = &m_Camera;


     m_InputSystem.Init(m_Window->GetHandle());
     m_Renderer = new Renderer(desc, m_Window->GetHandle(), &m_InputSystem,&m_AssetManager);


     m_GUIRenderer = new GUIRenderer(this,false);
     m_AssetManager.Init(this);
   //  m_AssetManager.LoadAllResources("C:\\Repos\\VulkanEngine\\Resources\\Textures\\",ResourceType::TEXTURE);
     m_AssetManager.LoadAllResources("C:\\Repos\\VulkanEngine\\Resources\\Animation\\", ResourceType::ANIMATION);

     //m_AssetManager.LoadAllResources("C:\\Repos\\VulkanEngine\\Resources\\Textures\\", ResourceType::TEXTUREATLAS);


     m_Renderer->InitializePipeline(m_AssetManager.GetResourceCount(ResourceType::TEXTURE));

 }

 void Application::AddLayer(Layer* layer)
 {
     layer->SetSystem(this);
    m_LayerController.CreateLayer(layer);
 }

 Float2 Application::GetMousePos()
 {
     return m_InputSystem.GetMousePos();
 }

 Float2 Application::GetMousePosNorm()
 {
 
     return { m_InputSystem.GetMousePos().x/m_Renderer->GetViewPortExtent().width,1.0f-(m_InputSystem.GetMousePos().y / m_Renderer->GetViewPortExtent().height )};
 }

 Float2 Application::GetWorldMousePos()
 {
     return m_InputSystem.GetWorldMousePos(m_Camera.GetPosition(), m_Camera.GetScale(), { (float)m_Renderer->GetViewPortExtent().width,(float)m_Renderer->GetViewPortExtent().height });
 }

 GUUID Application::GetCurrentlyHoveredPixelID()
 {
    Buffer* buffer =  m_Renderer->GetCustomBuffer(0);
    Float2 MousePos = GetMousePos();

    Float2 RawID = buffer->ReadPixel((uint32_t)MousePos.x, (uint32_t)MousePos.y,m_Renderer->GetViewPortExtent().width, m_Renderer->GetViewPortExtent().height);
    uint64_t* ID = (uint64_t*)&RawID;
     return GUUID(*ID);
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
        m_InputSystem.ResetMouseChange();

    }
 }

 