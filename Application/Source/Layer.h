#pragma once
#include "AppCore.h"
class Application;
class Renderer;
class AssetManager;
class Layer{
public:
    virtual void OnUpdate(float deltatime)=0;
    virtual void OnRender(double deltaTime)=0;
    virtual void OnCreate()=0;
    virtual void OnDestroy()=0;
    virtual void OnGUI()=0;

    void Init();
protected:
    Layer(std::string name);

    std::string m_LayerName{};
public:
};
class LayerController{
    public:
       
        Layer* CreateLayer(Layer* layer);
        void UpdateLayers(double deltatime);
        void RenderLayers(double deltaTime);
        void UpdateGUILayers();
        void DestroyLayers();
    private:
    std::vector<Layer*> m_Layers{};
};
