#pragma once
#include "Application.h"
#include "DLLLoading.h"
class DLLLoadingLayer:public Layer
{
public:
    DLLLoadingLayer();
    void OnCreate()override;
    void OnUpdate(double deltatime)override;
    void OnGUI()override;
    void OnDestroy()override;
	void OnRender(double deltime)override;

private:
    Core::DLLFunc<int(*)(int,int)> m_DllAdder;
};

