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
            Core::Log("Has this asset Type:",(uint32_t)Application::GetAssetType(IdOfAsset));
        else 
            Core::Log("This asset isn't loaded to asset manager.");

    }
    else if(Command == "help"){
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

bool Application::InitApplicationBackEnd(ApplicationSpecs specs){

    if(specs.OpenTerminal)
        OpenTerminalAndAttachToStream();

    tcgetattr(STDIN_FILENO,&m_DefaultConsoleSett);

     Core::EmptyLogFile();
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
  

     m_Renderer->InitializePipeline(500);

     m_FontSystem = new FontSystem();

     m_GUIRenderer = new GUIRenderer(this, false);
     if(specs.IsWindowResizable){
        glfwSetFramebufferSizeCallback(m_Window->GetHandle(),DefaultWindowResizeCallback);
        
     }

     return true;

 }
     bool Application::DeleteApplication(){
        delete Application::GetApplication();
        return true;
    }

Float2 Application::GetMousePosChange(){
    Application* app = Application::GetApplication();
    return app->m_InputSystem.GetMousePosChange();
}
 void Application::AddLayer(Layer* layer)
 {
    Application* app = GetApplication();
     layer->Init();
    app->m_LayerController.CreateLayer(layer);
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
    Buffer* buffer =  app->m_Renderer->GetCustomBuffer(0);
    Float2 MousePos = GetMousePos();

    Float2 RawID = buffer->ReadPixel((uint32_t)MousePos.x, (uint32_t)MousePos.y,app->m_Renderer->GetViewPortExtent().width, app->m_Renderer->GetViewPortExtent().height);
    uint64_t* ID = (uint64_t*)&RawID;
     return GUUID(*ID);
 }


void Application::Shutdown(){
    Application* app = GetApplication();
    app->m_Running = false;
    //set the  font color of cout to default.
    tcsetattr(STDIN_FILENO,TCSANOW,&app->m_DefaultConsoleSett);
    std::cout << "\033[0m"<<std::flush;
    std::cout << "Shutting Down...";

}


 void Application::LoadAssets(std::string Path, AssetType type)
 {
    Application* app = GetApplication();
     app->m_AssetManager.LoadAllAssets(Path, type);
 }
     Collider Application::CreateCollider(const Float2& position,const Float2& size){
        Application* app = Application::GetApplication();
        return app->m_CollisionSystem.CreateCollider(position,size);
    }
void Application::RunCollision(){
    Application* app= Application::GetApplication();

    app->m_CollisionSystem.RunCollisions();
  
}
void Application::RunAStar(){

}
 void Application::Run(){
    std::atomic<bool> ThreadRunning(true);
    Application* app = GetApplication();
    app->m_Running = true;

    std::thread InputThread(RunCommandLineInputTemp,app,std::ref(ThreadRunning));

    while(!glfwWindowShouldClose(app->m_Window->GetHandle())&& app->m_Running){
        app->m_InputSystem.ResetMouseChange();

        app->m_DeltaTime = Time::GetTimeMs() - app->m_LastFrameTime;
        app->m_LastFrameTime = Time::GetTimeMs();
        app->m_Renderer->BeginFrame(&app->m_Camera,app->m_DeltaTime);

        if(app->m_Renderer->GetSwapChainState() == VK_SUCCESS){

        app->m_LayerController.UpdateLayers(app->m_DeltaTime);
        app->m_LayerController.RenderLayers(app->m_DeltaTime);

        app->m_GUIRenderer->BeginGUI();
        app->m_Renderer->BeginGUIFrame();

        app->m_LayerController.UpdateGUILayers();
        
          if(app->m_RendererDebugging){
            app->m_Renderer->Statistics(true,app->m_GUIRenderer);
        }

        app->m_Renderer->EndFrame();
    }
        RunCollision();
        RunAStar();

      

        app->m_InputSystem.ResetInput();

        glfwSwapBuffers(app->m_Window->GetHandle());
        glfwPollEvents();
        
  
    }
    ThreadRunning.store(false);
    app->m_Running = false;
    InputThread.join();
    
    tcsetattr(STDIN_FILENO,TCSANOW,&app->m_DefaultConsoleSett);
    Shutdown();

}


 void Application::AddCallback( InputCallbacks* callbacks)
 {
    Application* app = GetApplication();
     app->m_InputSystem.AddCallbacks(callbacks);
 }
    Application::~Application(){
        Shutdown();
        delete m_Window;
        delete m_Renderer;
        delete m_FontSystem;
        delete m_GUIRenderer;
        glfwTerminate();
    }
    void Application::OpenTerminalAndAttachToStream(){
        //linux
       Core::Log("Not yet implemented");
    }


 