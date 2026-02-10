#define DEMO_FEATURES
#include  "../Examples/Testing/CollisionTestingLayer.h"
#include  "../Examples/Testing/GUITesting.h"
#include "../Examples/Testing/TestingLayer.h"
int main() {
	
	ApplicationSpecs specs{};
	specs.WindowTitle = "Example";
	specs.WindowHeight = 800;
	specs.WindowWidth = 800;
	specs.ClearColor = { 0.5f,0.5f,0.5f,1.0f };
	specs.RendererDebugging = true;
	specs.IsWindowResizable = true;
	specs.ApplicationVersion = 0.001f;
	specs.AppDebugging = true;
	Application::InitApplication(specs);
	Application::AddLayer<GUITestingLayer>();

	Application::Run();
	


    return 0;
}