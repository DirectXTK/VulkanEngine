#include "Application.h"
#include "AppTime.h"
#include "AssetManager.h"
void ChechCommands(Application* app,std::string Command){
    if(Command == "prtassets"){
        app->GetAssetManager()->DebugStatistics(false);
    }else if(Command == "help"){
        Core::Log("prtassets -prints all loaded assets");
        Core::Log("exit -exits the program");
    }else if(Command == "exit"){
        app->Shutdown();
    }else{
        Core::Log("Invalid command");
    }
}

void RunCommandLineInputTemp(Application* inapp,std::atomic<bool>& threadRunning){
    std::string input{};
    while(threadRunning.load()){

        input =Core::ReadInputFromConsole(threadRunning);

        ChechCommands(inapp,input);

    
    if(input == "exit")
        return;
    }
}
 Application::Application(ApplicationSpecs specs){
    //init glfw
     //m_ApplicationLayer = (ApplicationLayer*)m_LayerController.CreateLayer(new ApplicationLayer(specs));

    tcgetattr(STDIN_FILENO,&m_DefaultConsoleSett);

     Core::EmptyLogFile();
     glfwInit();
     glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
     glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_RendererDebugging = specs.RendererDebugging;
     m_Window = new Window(specs.WindowWidth, specs.WindowHeight, specs.WindowTitle);
     m_Camera.Init({ 0.0f,0.0f }, { 1.0f,1.0f });

     RendererDesc desc{};
     desc.VertexCountPerDrawCall = 4*1000;
     desc.ClearColor = specs.ClearColor;
     desc.InitialCamera = &m_Camera;

     m_InputSystem.Init(m_Window->GetHandle());

     m_Renderer = new Renderer(desc, m_Window->GetHandle(), &m_InputSystem,&m_AssetManager);

     m_AssetManager.Init(this);
  

     m_Renderer->InitializePipeline(500);


     m_FontSystem = new FontSystem(this);
     m_GUIRenderer = new GUIRenderer(this, false);

 }

 void Application::AddLayer(Layer* layer)
 {
     layer->Init(this,&m_AssetManager,m_Renderer);
    m_LayerController.CreateLayer(layer);
 }

 Float2 Application::GetMousePos()
 {
     return m_InputSystem.GetMousePos();
 }

 Float2 Application::GetMousePosNorm()
 {
 
     return { (m_InputSystem.GetMousePos().x/m_Renderer->GetViewPortExtent().width*2.f)-1.0f,1.0f-(m_InputSystem.GetMousePos().y / m_Renderer->GetViewPortExtent().height*2.0f )};
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


void Application::Shutdown(){
    m_Running = false;
    //set the  font color of cout to default.
    tcsetattr(STDIN_FILENO,TCSANOW,&m_DefaultConsoleSett);
    std::cout << "\033[0m"<<std::flush;
    std::cout << "Shutting Down...";

}


 void Application::LoadAssets(std::string Path, AssetType type)
 {
     m_AssetManager.LoadAllAssets(Path, type);
 }

 void Application::Run(){
    std::atomic<bool> ThreadRunning(true);
    m_Running = true;

    std::thread InputThread(RunCommandLineInputTemp,this,std::ref(ThreadRunning));

    while(!glfwWindowShouldClose(m_Window->GetHandle())&& m_Running){
        m_InputSystem.ResetMouseChange();

        m_DeltaTime = Time::GetTimeMs() - m_LastFrameTime;
        m_LastFrameTime = Time::GetTimeMs();
        m_Renderer->BeginFrame(&m_Camera,m_DeltaTime);

        m_LayerController.UpdateLayers(m_DeltaTime);

        m_GUIRenderer->BeginGUI();
        m_Renderer->BeginGUIFrame();

        m_LayerController.UpdateGUILayers();
        
          if(m_RendererDebugging){
            m_Renderer->Statistics(true,m_GUIRenderer);
        }
        m_Renderer->EndFrame();

      

        m_InputSystem.ResetInput();

        glfwSwapBuffers(m_Window->GetHandle());
        glfwPollEvents();
        
  

    }
    ThreadRunning.store(false);
    m_Running = false;
    InputThread.join();
    
    tcsetattr(STDIN_FILENO,TCSANOW,&m_DefaultConsoleSett);
    Shutdown();

}

 Collider Application::CreateCollider(Float2* Position,Float2* Size)
 {
    Collider collider = m_CollisionSystem.CreateCollider();
    collider.Update(Position,Size);
     return Collider();
 }

 void Application::AddCallback( InputCallbacks* callbacks)
 {
     m_InputSystem.AddCallbacks(callbacks);
 }
    Application::~Application(){
        Shutdown();
        delete m_Window;
        delete m_Renderer;
        delete m_FontSystem;
        delete m_GUIRenderer;
        glfwTerminate();
    }


 