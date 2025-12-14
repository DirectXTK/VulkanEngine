#pragma once
#include "AppCore.h"
#include <Renderer.h>
#include "Window.h"
#include "InputSystem.h"
#include "Layer.h"
#include "AssetManager.h"
#include "Collider.h"
#include "FontSystem.h"
#include "Collider.h"
#include "Event.h"

#include "PathAgent.h"

struct ApplicationSpecs{
    uint32_t WindowHeight{500};
     uint32_t WindowWidth{500};
    std::string WindowTitle{"Untitled"};
    bool RendererDebugging{false};
    bool IsWindowResizable{true};
    bool OpenTerminal{false};
    bool AppDebugging{false};

    //Renderer stuff
    Float4 ClearColor{ 0.5f,0.5f,0.5f,1.0f };
};

class Application {
public:
    Application();
    static bool InitApplication(ApplicationSpecs specs){m_Application = new Application();return m_Application->InitApplicationBackEnd(specs);}
    static bool InitApplication(Application* app){m_Application = app;return true;}

    template<typename T>
    static void AddLayer(){
         Application* app = GetApplication();
         app->m_LayerController.QueueCreateLayer(new T());
    }
    template<typename REMOVEL>
    static void RemoveLayer(){Application::GetApplication()->m_LayerController.QueueRemoveLayer<REMOVEL>();}
    template<typename LAYER>
    static LAYER* GetLayer(){Application::GetApplication()->m_LayerController.GetLayer<LAYER>();}

    static float GetDeltaTime() { return (float)GetApplication()->m_DeltaTime; }

    static GUUID GetCurrentlyHoveredPixelID();
    static AssetManager* GetAssetManager() { return &GetApplication()->m_AssetManager; }
    static Camera2D* GetCurrentCamera(){return &Application::GetApplication()->m_Camera;}
    static Renderer* GetRenderer(){return GetApplication()->m_Renderer;}
    static GUIRenderer* GetGUIRenderer(){return GetApplication()->m_GUIRenderer;}


    static void LoadAssets(std::string Path,AssetType type);
    static AssetType GetAssetType(GUUID id);
    static bool HasAsset(GUUID id);
    template<typename ASSETTYPE>
    static Asset<ASSETTYPE> GetAsset(const std::string& strHandle){return GetApplication()->m_AssetManager.GetAsset<ASSETTYPE>(strHandle);}


    static void Run();
    static void QueueShutDown();
    //Input
    static void DispatchEvent(Event& event);
    static bool IsMouseClicked(const MouseCodes& codes,bool hold= false);
    static bool IsKeyPressed(const KeyCodes& codes);
    static bool IsKeyReleased(const KeyCodes& codes);    
    static float GetScroll();
    static Float2 GetMousePos();
    static Float2 GetMousePosChange();
    static Float2 GetMousePosNorm();
    static Float2 GetWorldMousePos();
    //Assets
    static uint64_t GetAssetCount(const AssetType& type); 

    //Collision
    //returns true if collided.
    static bool RunCollision(Float2& fPos,Float2& fSize,Float2& sPos,Float2& sSize);
    //position needs to be Float2 and size must be Float2.
    static void RunCollisionAsync(void* objData,uint32_t posOffset,uint32_t sizeOffset,uint64_t objCount,uint64_t stride,int32_t isCollidedOffset=-1);

    static void RunAStar();

    static PathAgentHandle CreatePathAgent(const Float2& position,const Float2& size,const AgentType& type);

    static Application* GetApplication(){return m_Application;}
    static bool DeleteApplication();
    ~Application();
private:
//has to be non static
     bool InitApplicationBackEnd(ApplicationSpecs specs);
    static void OpenTerminalAndAttachToStream();

    static void Shutdown();

    //Debug
   static void LogSystemAndAppInformation();
    static Application* m_Application;



    Window* m_Window{};

    LayerController m_LayerController{ };

    double m_DeltaTime{};
    double m_LastFrameTime{};
    ColliderSystem m_CollisionSystem{};
    //multithreading
    std::vector<std::thread> m_ThreadPool{};

public:
    //GUI Renderer
    Renderer* m_Renderer{};
    Camera2D m_Camera{};
    InputSystem m_InputSystem{};
    AssetManager m_AssetManager{};
    bool m_Running{};
    //Debugging
    bool m_RendererDebugging{false};
    ApplicationSpecs m_Specs{};
    //console
    termios m_DefaultConsoleSett{};

#ifdef GUI_HEADER
    FontSystem* m_FontSystem{};
    GUIRenderer* m_GUIRenderer{};
#endif
    PathSystem* m_PathFinderSystem{};
};