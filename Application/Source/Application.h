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
    Float4 ClearColor{ 1.0f,1.0f,1.0f,1.0f };
};


class Application {
public:
    Application(ApplicationSpecs specs);
    void AddLayer(Layer* layer);
    float GetDeltaTime() { return m_DeltaTime; }

    Float2 GetMousePos();
    Float2 GetMousePosNorm();
    Float2 GetWorldMousePos();

    GUUID GetCurrentlyHoveredPixelID();

    template<typename T>
    T* GetResource(GUUID id) { return m_AssetManager.GetResource<T>(id); }
    template<typename T>
    T* GetResource(const std::string& Path) { return m_AssetManager.GetResource<T>(Core::GetStringHash(Path)); }

    AssetManager* GetAssetManager() { return &m_AssetManager; }
    void LoadAssets(std::string Path,ResourceType type);

    void Run();
    Collider CreateCollider(Float2* Position, Float2 *Size);

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