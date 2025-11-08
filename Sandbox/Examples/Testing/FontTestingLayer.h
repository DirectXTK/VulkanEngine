#pragma once
#define DEMO_FEATURES
#include "Application.h"
#include "Utils.h"

class FontTestingLayer:public Layer
{
public:
	FontTestingLayer();
	void OnCreate()override;
	void OnUpdate(double DeltaTime)override;
	void OnGUI()override;
	void OnDestroy()override;
	void OnRender(double deltime)override;

private:
	FontSystem* m_FontSystem{};
};

