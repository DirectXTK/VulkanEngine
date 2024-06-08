#include "GUITesting.h"
#include "Serializer.h"

GUITestingLayer::GUITestingLayer() : Layer("GUITestingLayer")
{

}
void GUITestingLayer::OnCreate() {

   

}
void GUITestingLayer::OnUpdate(double deltatime)
{
   

}

void GUITestingLayer::OnGUI()
{
	static float Position{ -0.8f };
	static float Position2{-0.1f};

	


	m_App->m_GUIRenderer->Panel("Panel", {Position,0.0f}, {1.0f,1.0f,1.0f,1.0f}, {0.2f,1.0f}, 0, true);

	

	m_App->m_GUIRenderer->EndPanel();

	static Float2 CurrentPos{ m_App->m_InputSystem.GetMousePos() };

	CurrentPos.x += m_App->m_InputSystem.GetMousePosChange().x;
	CurrentPos.y += m_App->m_InputSystem.GetMousePosChange().y;

	//Core::Log(ErrorType::Info, "Change ", m_App->m_InputSystem.GetMousePosChange().x);

	if (m_App->m_InputSystem.IsKeyPressed(KeyCodes::X)) {
		Core::Log(ErrorType::Info, "Real position", m_App->m_InputSystem.GetMousePos().x, " ", m_App->m_InputSystem.GetMousePos().y);
		Core::Log(ErrorType::Info, "CHangePos position", CurrentPos.x, " ", CurrentPos.y);
	}
}

void GUITestingLayer::OnDestroy()
{
}



