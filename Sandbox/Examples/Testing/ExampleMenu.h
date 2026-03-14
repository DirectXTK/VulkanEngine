#pragma once
#include "Application.h"
class ExampleMenu: public Layer
{
public:
	ExampleMenu();
	void OnCreate()override;
	void OnUpdate(double DeltaTime)override;
	void OnDestroy()override;
	void OnGUI()override;
	void OnRender(double deltime)override;

private:

};



