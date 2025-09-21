#define DEMO_FEATURES
//#include "../Examples/Testing/GUITesting.h"
#include "../Examples/Testing/FontTestingLayer.h"
#include "../Examples/Testing/TextureTestingLayer.h"
#include "../Examples/Testing/AnimationTestingLayer.h"

#define DEBUG

int main() {
	ApplicationSpecs specs{};
	specs.WindowTitle = "Example";
	specs.WindowHeight = 800;
	specs.WindowWidth = 800;
	specs.ClearColor = { 0.5f,0.5f,0.5f,1.0f };
	specs.RendererDebugging = true;

	Application::InitApplication(specs);
	Application::AddLayer(new AnimationTestingLayer());
	Application::Run();
   

    return 0;
}