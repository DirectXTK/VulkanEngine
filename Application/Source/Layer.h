#pragma once
#include "AppCore.h"
class Renderer;
class Camera2D;
class Layer{
public:
    virtual void OnUpdate()=0;
    virtual void OnCreate()=0;
    virtual void OnDestroy()=0;
    virtual void OnGUI()=0;

    void SetSystem(Camera2D* camera, Renderer* renderer);
protected:
    Layer(std::string name);

    std::string m_LayerName{};
    Renderer* m_Renderer{};
    Camera2D* m_Camera2{};
};
class LayerController{
    public:
       
        Layer* CreateLayer(Layer* layer);
        void UpdateLayers();
        void DestroyLayers();
    private:
    std::vector<Layer*> m_Layers{};
};
