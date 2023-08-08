#pragma once
#include "AppCore.h"
class Layer{
public:
    Layer(std::string name);
    virtual void OnUpdate()=0;
    virtual void OnCreate()=0;
    virtual void OnDestroy()=0;
    virtual void OnGUI()=0;

private:
    std::string m_LayerName{};
};
class LayerController{
    public:
        LayerController(){}
        void AddLayer();

    private:
    std::vector<Layer> m_Layers{};
};
