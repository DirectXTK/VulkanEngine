#pragma once
#include "Application.h"

class MultiThreadingLayer : public Layer{
    public:
    MultiThreadingLayer();
    void OnCreate()override;
    void OnUpdate(double deltaTime)override;
    void OnRender(double delaTime)override;
    void OnDestroy()override;
    void OnGUI()override;


    private:
};