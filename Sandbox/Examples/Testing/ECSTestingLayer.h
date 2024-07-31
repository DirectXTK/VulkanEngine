#pragma once
#define DEMO_FEATURES
#include "Utils.h"
#include "Application.h"
class ECSTestingLayer :public Layer
{
public:
	ECSTestingLayer();
	void OnCreate()override;
	void OnUpdate(double DeltaTime)override;
	void OnDestroy()override;
	void OnGUI()override;
private:
	
};

