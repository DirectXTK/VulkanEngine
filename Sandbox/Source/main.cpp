#define DEMO_FEATURES
#include  "../Examples/Testing/GUITesting.h"

int main() {
	ApplicationSpecs specs{};
	specs.WindowTitle = "Example";
	specs.WindowHeight = 800;
	specs.WindowWidth = 800;
	specs.ClearColor = { 0.5f,0.5f,0.5f,1.0f };
	specs.RendererDebugging = true;
	specs.IsWindowResizable = true;
	Application::InitApplication(specs);
	Application::AddLayer<GUITestingLayer>();

	Application::Run();
   

    return 0;
}