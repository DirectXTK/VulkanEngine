#pragma once
#include "AppCore.h"
class Application;
class Layer{
public:
    virtual void OnUpdate(double deltatime)=0;
    virtual void OnCreate()=0;
    virtual void OnDestroy()=0;
    virtual void OnGUI()=0;

    void SetSystem(Application* app);
protected:
    Layer(std::string name);

    std::string m_LayerName{};
public:
    Application* m_App{};
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
