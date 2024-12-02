#pragma once
#include "Application.h"
class DLLLoadingLayer:public Layer
{
public:
    DLLLoadingLayer();
    void OnCreate()override;
    void OnUpdate(float deltatime)override;
    void OnGUI()override;
    void OnDestroy()override;


private:
};

