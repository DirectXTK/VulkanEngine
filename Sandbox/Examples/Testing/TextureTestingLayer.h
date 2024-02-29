#pragma once
#include "Application.h"
class TextureTestingLayer: public Layer
{
public :
	TextureTestingLayer();
	void OnCreate()override;
	void OnUpdate(double DeltaTime)override;
	void OnGUI()override;
	void OnDestroy()override;
private:
};

