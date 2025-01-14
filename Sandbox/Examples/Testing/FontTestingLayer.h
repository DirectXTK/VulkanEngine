#pragma once
#define DEMO_FEATURES
#include "Application.h"
#include "Utils.h"

class FontTestingLayer:public Layer
{
public:
	FontTestingLayer();
	void OnCreate()override;
	void OnUpdate(float DeltaTime)override;
	void OnGUI()override;
	void OnDestroy()override;

private:
	FontSystem* m_FontSystem{};
};

