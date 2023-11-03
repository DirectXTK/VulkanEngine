#include "GUI.h"
#include "Renderer.h"
#include "Application.h"
#include "RandomGenerator.h"
GUIRenderer::GUIRenderer(Application* app): m_Application(app)
{
}
void GUIRenderer::BeginGUI()
{
	m_CurrentButton = 0;
}
bool GUIRenderer::Button(Float2 Position,Float4 Color,Float2 Size)
{
	Renderer* renderer = m_Application->m_Renderer;
	InputSystem* inputsystem = &m_Application->m_InputSystem;
	uint64_t* id{};

	if (m_CurrentButton == m_ButtonIDs.size())
		m_ButtonIDs.push_back({ Core::RandomUInt64(0, std::numeric_limits<uint64_t>::max()) });
	renderer->DrawQuad({ Position.x,Position.y,0.0f }, Color, Size, nullptr, m_ButtonIDs[m_CurrentButton].ID);

	m_CurrentButton++;
	if (inputsystem->IsMouseClicked(MouseCodes::LEFT))
	{
		Buffer* buffer = renderer->GetCustomBuffer(0);
		Float2 data = buffer->ReadPixel((uint32_t)inputsystem->GetMousePos().x, (uint32_t)inputsystem->GetMousePos().y, renderer->GetViewPortExtent().width, renderer->GetViewPortExtent().height);
		Float2 Pos = inputsystem->GetMousePos();
		id = (uint64_t*)&data;
		if (m_ButtonIDs[m_CurrentButton-1].ID == *id)
			return true;
	}
	return false;
}
void GUIRenderer::EndGUI()
{
	//delete button that hasn't been used.
}

