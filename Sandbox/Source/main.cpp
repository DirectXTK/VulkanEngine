#define DEMO_FEATURES
#include "../Examples/Testing/GUITesting.h"
int main() {
	ApplicationSpecs specs{};
	specs.WindowTitle = "Example";
	specs.WindowHeight = 500;
	specs.WindowWidth = 500;
	specs.ClearColor = { 0.5f,0.5f,0.5f,1.0f };

	Application app(specs);
	app.AddLayer(new GUITestingLayer());
	app.Run();
   
    return 1;
}