#pragma once
#include "Application.h"
class ExampleMenu: public Layer
{
public:
	ExampleMenu();
	void OnCreate()override;
	void OnUpdate(float DeltaTime)override;
	void OnDestroy()override;
	void OnGUI()override;
private:

};

void RunExampleMenu() {
	ApplicationSpecs specs{};
	specs.WindowTitle = "Example";
	specs.WindowHeight = 500;
	specs.WindowWidth = 500;

	Application app(specs);
	app.AddLayer(new ExampleMenu());
	app.Run();

}

