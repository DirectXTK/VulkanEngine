#define DEMO_FEATURES
//#include "../Examples/Testing/GUITesting.h"
#include "../Examples/Testing/FontTestingLayer.h"
#include "../Examples/Testing/TextureTestingLayer.h"

int main() {
	ApplicationSpecs specs{};
	specs.WindowTitle = "Example";
	specs.WindowHeight = 500;
	specs.WindowWidth = 500;
	specs.ClearColor = { 0.5f,0.5f,0.5f,1.0f };

	Application app(specs);
	app.AddLayer(new TextureTestingLayer());
	app.Run();
   

    return 1;
}