#include "Application.h"
#include "AppTime.h"
#include "AssetManager.h"
 Application* Application::m_Application = nullptr;
void ChechCommands(Application* app,std::string Command){
    uint64_t argsStart = Command.find(" ");
    std::string Args{};
    if(argsStart == (uint64_t)-1){
        argsStart =0;
    }else{
        Args= Command.substr(argsStart,Command.size()-argsStart);
        Command = Command.substr(0,argsStart);
    }

    if(Command == "prtassets"){
        app->GetAssetManager()->DebugStatistics(false);
    }
    else if(Command == "find"){
        Core::Log(Args);
        uint64_t IdOfAsset = std::stoul(Args);
        if(Application::HasAsset(IdOfAsset))
            Core::Log("Has this asset Type:",Core::GetAssetTypeString(Application::GetAssetType(IdOfAsset)));

        else 
            Core::Log("This asset isn't loaded to asset manager.");

    }
    else if(Command == "help"){
        Core::Log("prtassets -prints all loaded assets");
        Core::Log("exit -exits the program");
    }else if(Command == "exit"){
        app->QueueShutDown();
    }else{
        Core::Log("Invalid command");
    }
}
void Application::LoadAllAssets(const std::string& path,const AssetType& typeToLoad){
    Application* app = Application::GetApplication();
    app->m_AssetManager.LoadAllAssets(path,typeToLoad);
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
AssetType Application::GetAssetType(GUUID id){
     Application* app = GetApplication();
    return app->GetAssetManager()->GetAssetType(id);
}
bool Application::HasAsset(GUUID id){
    Application* app = GetApplication();
    return app->GetAssetManager()->HasAsset(id);
}

bool Application::IsMouseClicked(const MouseCodes& codes,bool hold){
    Application* app = GetApplication();
    return app->m_InputSystem.IsMouseClicked(codes,hold);
} 
 bool Application::IsKeyPressed(const KeyCodes& codes){
      Application* app = GetApplication();
   return app->m_InputSystem.IsKeyPressed(codes);
}

 bool Application::IsKeyReleased(const KeyCodes& codes){
      Application* app = GetApplication();
    return app->m_InputSystem.IsKeyReleased(codes);
 }

 float Application::GetScroll(){
      Application* app = GetApplication();
    return app->m_InputSystem.GetScroll();
 }

Application::Application(){}
void Application::LogSystemAndAppInformation(){
    Core::Log("App information");
    std::cout << "  Build "<< Core::GetBuildConfiguration()<<"\n";
    std::cout << "  App version "<< GetApplicationSpecs().ApplicationVersion<<"\n";
    #ifdef __linux__
    std::cout << "  Platform LINUX\n";
    #elif __win32__
    std::cout << "  Platform WINDOWS\n";
    #endif
    Core::Log("Computer information");
    std::cout << "  RAM "<<Core::GetTotalAmountOfRam()/1024/1024<<"MB\n";
    std::cout << "  CPU "<<Core::GetCPUName()<<"\n";
    std::cout << "  Thread count "<<Core::GetCPUThreadCount()<<"\n";
    std::cout << "  Core count "<<Core::GetCPUCoreCount()<<"\n";
    std::cout << "  SSE "<<Core::GetCPUSupportedInscrutionSets().SSE<<"\n";
    std::cout << "  SSE2 "<<Core::GetCPUSupportedInscrutionSets().SSE2<<"\n";
    std::cout << "  SSE3 "<<Core::GetCPUSupportedInscrutionSets().SSE3<<"\n";
    std::cout << "  SSE4.1 "<<Core::GetCPUSupportedInscrutionSets().SSE41<<"\n";
    std::cout << "  SSE4.2 "<<Core::GetCPUSupportedInscrutionSets().SSE42<<"\n";
    std::cout << "  GPU "<<Core::GetGPUName()<<"\n";



}
bool Application::InitApplicationBackEnd(ApplicationSpecs specs){

    m_Specs = specs;

    if(specs.AppDebugging)
        LogSystemAndAppInformation();
    if(specs.OpenTerminal)
        OpenTerminalAndAttachToStream();
    m_ThreadPool.resize(Core::GetCPUThreadCount());

    tcgetattr(STDIN_FILENO,&m_DefaultConsoleSett);

     Core::EmptyLogFile();

#ifdef LINUXX11
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
#ifdef LINUXWAYLAND
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
#endif
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

     glfwInit();
     glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
     glfwWindowHint(GLFW_RESIZABLE, specs.IsWindowResizable);

    m_RendererDebugging = specs.RendererDebugging;

     m_Window = new Window(specs.WindowWidth, specs.WindowHeight, specs.WindowTitle);

     m_Camera.Init({ 0.0f,0.0f }, { 1.0f,1.0f },{(float)specs.WindowWidth,(float)specs.WindowHeight});

     RendererDesc desc{};
     desc.VertexCountPerDrawCall = 4*1000;
     desc.ClearColor = specs.ClearColor;
     desc.InitialCamera = &m_Camera;

     m_InputSystem.Init(m_Window->GetHandle());

     m_Renderer = new Renderer(desc, m_Window->GetHandle(), &m_InputSystem,&m_AssetManager);

     m_AssetManager.Init(this);
  
     //Init a* path finding algorith system
    // m_PathFinderSystem = new PathSystem(1000,1000,{0.025f,0.025f});

     m_Renderer->InitializePipeline(500);

     m_FontSystem = new FontSystem();

     m_GUIRenderer = new GUIRenderer(this, false);

     m_AssetManager.LoadAllAssets("/users/jimy/Repos/VulkanEngine/EngineResources/",AssetType::TEXTURE);

     m_PickBuffer = new Float2[m_Renderer->GetViewPortExtent().width*m_Renderer->GetViewPortExtent().height];
     m_PickBufferSize = m_Renderer->GetViewPortExtent().width*m_Renderer->GetViewPortExtent().height*sizeof(Float2);

     return true;

 }
     bool Application::DeleteApplication(){
        delete Application::GetApplication();
        return true;
    }
PathAgentHandle Application::CreatePathAgent(const Float2& position,const Float2& size,const AgentType& type){
    Application* app = Application::GetApplication();
    return app->m_PathFinderSystem->CreateAgent(position,size,type);
}
Float2 Application::GetMousePosChange(){
    Application* app = Application::GetApplication();
    return app->m_InputSystem.GetMousePosChange();
}
    uint64_t Application::GetAssetCount(const AssetType& type){
        Application* app = Application::GetApplication();
        return app->m_AssetManager.GetAssetCount(type);
    } 

 Float2 Application::GetMousePos()
 {
    Application* app = GetApplication();
     return app->m_InputSystem.GetMousePos();
 }

 Float2 Application::GetMousePosNorm()
 {
    Application* app = GetApplication();
     return { (app->m_InputSystem.GetMousePos().x/app->m_Renderer->GetViewPortExtent().width*2.f)-1.0f,1.0f-(app->m_InputSystem.GetMousePos().y / app->m_Renderer->GetViewPortExtent().height*2.0f )};
 }

 Float2 Application::GetWorldMousePos()
 {
    Application* app = GetApplication();
     return app->m_InputSystem.GetWorldMousePos(app->m_Camera.GetPosition(), app->m_Camera.GetScale(), { (float)app->m_Renderer->GetViewPortExtent().width,(float)app->m_Renderer->GetViewPortExtent().height });
 }

 GUUID Application::GetCurrentlyHoveredPixelID()
 {
    Application* app = GetApplication();
    Float2 MousePos = GetMousePos();
    Float2 rawID{};

    if(!Core::ReadPixel(app->m_PickBuffer,app->m_Renderer->GetViewPortExtent().width,app->m_Renderer->GetViewPortExtent().height,MousePos.x,MousePos.y,&rawID)){
        Core::Log(ErrorType::Error,"Failed to read pixel Application::GetCurrentlyHoveredPixelID","MouseX:",MousePos.x," MouseY:",MousePos.y," Width:",app->m_Renderer->GetViewPortExtent().width," Height:" ,app->m_Renderer->GetViewPortExtent().height);
        return GUUID(0);
    }
    uint64_t* ID = (uint64_t*)&rawID;
    Core::Log(*ID);
     return GUUID(*ID);
 }


void Application::Shutdown(){
    Application* app = GetApplication();
    
    //set the  font color of cout to default.
    tcsetattr(STDIN_FILENO,TCSANOW,&app->m_DefaultConsoleSett);
    std::cout << "\033[0m"<<std::flush;
    std::cout << "Shutting Down...\n";

    app->m_LayerController.DestroyLayers();
    app->m_Running = false;
    app->m_Renderer->FinishExecution();

    app->DeleteApplication();
}
void Application::QueueShutDown(){
    Application::GetApplication()->m_Running =false;
}

 void Application::LoadAssets(std::string Path, AssetType type)
 {
    Application* app = GetApplication();
     app->m_AssetManager.LoadAllAssets(Path, type);
 }

bool Application::RunCollision(Float2& fPos,Float2& fSize,Float2& sPos,Float2& sSize){
    Application* app= Application::GetApplication();
   return  app->m_CollisionSystem.DefaultCollisionFunction(fPos,fSize,sPos,sSize);
}
void Application::RunCollisionAsync(void* objData,uint32_t posOffset,uint32_t sizeOffset,uint64_t objCount,uint64_t stride,int32_t isCollidedOffset){
    Application* app= Application::GetApplication();
    app->m_CollisionSystem.RunCollisionsAsync((char*)objData,posOffset,sizeOffset,objCount,stride,isCollidedOffset);
}
void Application::RunAStar(){

}
 void Application::DispatchEvent(Event& event){
    Application* app = Application::GetApplication();

    if(event.GetEventType() == EventType::WINDOWRESIZE){
        delete[] app->m_PickBuffer;
        app->m_PickBufferSize = app->m_Renderer->GetViewPortExtent().width*app->m_Renderer->GetViewPortExtent().height*sizeof(Float2);
        app->m_PickBuffer = new Float2[app->m_PickBufferSize];
    }

    app->m_GUIRenderer->OnEvent(event);
    app->m_FontSystem->OnEvent(event);
    app->m_LayerController.OnEvent(event);
 }

 void Application::Run(){
    std::atomic<bool> ThreadRunning(true);
    Application* app = GetApplication();
    app->m_Running = true;

    std::thread InputThread(RunCommandLineInputTemp,app,std::ref(ThreadRunning));

    while(!glfwWindowShouldClose(app->m_Window->GetHandle())&& app->m_Running){

        app->m_Renderer->GetCustomBuffer(0)->LoadFromBufferToVar(app->m_PickBuffer,app->m_PickBufferSize,0);

        app->m_InputSystem.ResetMouseChange();

        app->m_DeltaTime = Time::GetTimeMs() - app->m_LastFrameTime;
        app->m_LastFrameTime = Time::GetTimeMs();
        app->m_Renderer->BeginFrame(&app->m_Camera,app->m_DeltaTime);

       // if(app->m_Specs.AppDebugging)
           // app->m_PathFinderSystem->RenderGrid();

        if(app->m_Renderer->GetSwapChainState() == VK_SUCCESS){
        
     //   app->m_PathFinderSystem->ResetGrid();
        app->m_LayerController.UpdateLayers(app->m_DeltaTime);
        app->m_LayerController.RenderLayers(app->m_DeltaTime);

        app->m_GUIRenderer->BeginGUI();
        app->m_Renderer->BeginGUIFrame();

        app->m_LayerController.UpdateGUILayers();
        
          if(app->m_RendererDebugging){
            app->m_Renderer->Statistics(true,app->m_GUIRenderer);
        }

        app->m_GUIRenderer->EndGUI();
        app->m_Renderer->EndFrame();
    }

      

        app->m_InputSystem.ResetInput();

        glfwSwapBuffers(app->m_Window->GetHandle());
        glfwPollEvents();
        
        app->m_LayerController.RunQueue();
        //app->m_LayerController.TransitionLayers();
        app->m_Renderer->RunRendererChangeQueue();
    }
    ThreadRunning.store(false);
    app->m_Running = false;
    InputThread.join();
    
    tcsetattr(STDIN_FILENO,TCSANOW,&app->m_DefaultConsoleSett);
    Shutdown();

}
    Application::~Application(){
        delete m_Window;

        delete m_FontSystem;
        delete m_GUIRenderer;
        m_AssetManager.Shutdown();
        delete m_Renderer;

        delete[] m_PickBuffer;
      //  delete m_PathFinderSystem;


        glfwTerminate();
    }
    void Application::OpenTerminalAndAttachToStream(){
        //linux
       Core::Log("Not yet implemented");
    }


 