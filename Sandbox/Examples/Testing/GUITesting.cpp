#include "GUITesting.h"
#include "Serializer.h"

GUITestingLayer::GUITestingLayer() : Layer("GUITestingLayer")
{

}
void GUITestingLayer::OnCreate() {

   

}
void GUITestingLayer::OnUpdate(float deltatime)
{
   

}
void GUITestingLayer::OnRender(double deltime){

}
void GUITestingLayer::OnGUI()
{
	static float Position{ -0.8f };
	static float Position2{-0.1f};
	GUIRenderer* guiRenderer = Application::GetGUIRenderer();
	


	guiRenderer->Panel("Panel", {Position,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.2f,1.0f}, 0, true);

	guiRenderer->Button("Start Button","Lafa", { 0.5f,0.5f }, { 1.0f,0.0f,0.0f,1.0f }, { 0.1f,0.1f });

	guiRenderer->EndPanel();

	static Float2 CurrentPos{ Application::GetMousePos() };

	CurrentPos.x += Application::GetMousePosChange().x;
	CurrentPos.y += Application::GetMousePosChange().y;

	//Core::Log(ErrorType::Info, "Change ", Application::m_InputSystem.GetMousePosChange().x);

	if (Application::IsKeyPressed(KeyCodes::X)) {
		Core::Log(ErrorType::Info, "Real position", Application::GetMousePos().x, " ", Application::GetMousePos().y);
		Core::Log(ErrorType::Info, "CHangePos position", CurrentPos.x, " ", CurrentPos.y);
	}
}

void GUITestingLayer::OnDestroy()
{
}



