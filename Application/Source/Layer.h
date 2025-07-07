#pragma once
#include "AppCore.h"
class Application;
class Renderer;
class AssetManager;
class Layer{
public:
    virtual void OnUpdate(float deltatime)=0;
    virtual void OnCreate()=0;
    virtual void OnDestroy()=0;
    virtual void OnGUI()=0;

    void Init(Application* app,AssetManager* manager,Renderer* renderer);
protected:
    Layer(std::string name);

    std::string m_LayerName{};
public:
    Application* m_App{};
    AssetManager* m_Assets{};
    Renderer* m_Renderer{};
};
class LayerController{
    public:
       
        Layer* CreateLayer(Layer* layer);
        void UpdateLayers(double deltatime);
        void UpdateGUILayers();
        void DestroyLayers();
    private:
    std::vector<Layer*> m_Layers{};
};
