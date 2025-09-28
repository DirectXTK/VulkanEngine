#pragma once
#include "AppCore.h"
#include <Renderer.h>
#include "Window.h"
#include "InputSystem.h"
#include "Layer.h"
#include "AssetManager.h"
#include "Collider.h"
#include "FontSystem.h"
struct ApplicationSpecs{
    uint32_t WindowHeight{500};
     uint32_t WindowWidth{500};
    std::string WindowTitle{"Untitled"};
    bool RendererDebugging{false};
    bool IsWindowResizable{true};

    //Renderer stuff
    Float4 ClearColor{ 0.5f,0.5f,0.5f,1.0f };
};

class Application {
public:
    Application();
    static bool InitApplication(ApplicationSpecs specs){m_Application = new Application();return m_Application->InitApplicationBackEnd(specs);}
    static void AddLayer(Layer* layer);
    static float GetDeltaTime() { return (float)GetApplication()->m_DeltaTime; }

    static GUUID GetCurrentlyHoveredPixelID();
    static AssetManager* GetAssetManager() { return &GetApplication()->m_AssetManager; }
    static Camera2D* GetCurrentCamera(){return &Application::GetApplication()->m_Camera;}
    static Renderer* GetRenderer(){return GetApplication()->m_Renderer;}
    static GUIRenderer* GetGUIRenderer(){return GetApplication()->m_GUIRenderer;}


    static void LoadAssets(std::string Path,AssetType type);
    static void Run();

    static Collider CreateCollider(Float2* Position, Float2 *Size);
    static void AddCallback( InputCallbacks* callbacks);
    static void Shutdown();
    //Input
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



    static Application* GetApplication(){return m_Application;}
    static bool DeleteApplication();
    ~Application();
private:
//has to be non static
     bool InitApplicationBackEnd(ApplicationSpecs specs);

    static Application* m_Application;



    Window* m_Window{};

    LayerController m_LayerController{ };

    double m_DeltaTime{};
    double m_LastFrameTime{};
    CollisionSystem m_CollisionSystem{};
public:
    //GUI Renderer
    Renderer* m_Renderer{};
    Camera2D m_Camera{};
    InputSystem m_InputSystem{};
    AssetManager m_AssetManager{};
    bool m_Running{};
    //Debugging
    bool m_RendererDebugging{false};
    //console
    termios m_DefaultConsoleSett{};

#ifdef GUI_HEADER
    FontSystem* m_FontSystem{};
    GUIRenderer* m_GUIRenderer{};
#endif

};