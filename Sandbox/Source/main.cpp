#define DEMO_FEATURES
#include  "../Examples/Testing/CollisionTestingLayer.h"

int main() {
	ApplicationSpecs specs{};
	specs.WindowTitle = "Example";
	specs.WindowHeight = 800;
	specs.WindowWidth = 800;
	specs.ClearColor = { 0.5f,0.5f,0.5f,1.0f };
	specs.RendererDebugging = true;
	specs.IsWindowResizable = true;
	specs.AppDebugging = true;
	Application::InitApplication(specs);
	Application::AddLayer<CollisionLayer>();

	Application::Run();
   

    return 0;
}