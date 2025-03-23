#pragma once
#include "AppCore.h"
#include "Renderer.h"
#include "Window.h"
#include "InputSystem.h"
#include "Layer.h"
#include "AssetManager.h"
#include "Collider.h"
struct ApplicationSpecs{
    uint32_t WindowHeight{500};
     uint32_t WindowWidth{500};
    std::string WindowTitle{"Untitled"};

    //Renderer stuff
    Float4 ClearColor{ 0.5f,0.5f,0.5f,1.0f };
};

class Application {
public:
    Application(ApplicationSpecs specs);
    void AddLayer(Layer* layer);
    float GetDeltaTime() { return (float)m_DeltaTime; }

    Float2 GetMousePos();
    Float2 GetMousePosNorm();
    Float2 GetWorldMousePos();

    GUUID GetCurrentlyHoveredPixelID();

    Asset GetAsset(GUUID id) { return m_AssetManager.GetAsset(id); }
    Asset GetAsset(const std::string& Path) { return m_AssetManager.GetAsset(Core::GetStringHash(Path)); }

    AssetManager* GetAssetManager() { return &m_AssetManager; }
    void LoadAssets(std::string Path,AssetType type);

    void Run();
    Collider CreateCollider(Float2* Position, Float2 *Size);
    void AddCallback( InputCallbacks* callbacks);
    
private:



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


#ifdef GUI_HEADER
    GUIRenderer* m_GUIRenderer{};
#endif

};