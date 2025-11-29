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

void RunExampleMenu() {
	ApplicationSpecs specs{};
	specs.WindowTitle = "Example";
	specs.WindowHeight = 500;
	specs.WindowWidth = 500;

	Application::InitApplication(specs);
	Application::AddLayer<ExampleMenu>();
	Application::Run();;

}

