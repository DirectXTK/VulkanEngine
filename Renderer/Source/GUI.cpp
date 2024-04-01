#include "GUI.h"
#include "Renderer.h"
#include "Application.h"
#include "RandomGenerator.h"
GUIRenderer::GUIRenderer(Application* app,bool SaveState): m_Application(app),m_SaveState(SaveState)
{
}
void GUIRenderer::BeginGUI()
{
	m_CurrentButton = 0;
	m_CurrentPanel = 0;
}
void GUIRenderer::Panel(Float2 Position, Float4 Color, Float2 Size, GUUID TextureHandle, bool Dragable)
{
	InputSystem* inputSystem = &m_Application->m_InputSystem;
	Renderer* renderer = m_Application->m_Renderer;
	if (m_CurrentPanel == m_PanelIDs.size()) {
		m_PanelIDs[m_CurrentPanel] = { {Position.x,Position.y},Size,Core::RandomUInt64(0, std::numeric_limits<uint64_t>::max()) };
	}

	if (!Dragable) {
		if (m_PanelDepth >= 1) {
			PanelData* PrevPanel = m_CurrenPanelParent;
			m_PanelIDs[m_CurrentPanel] = { { (Position.x * PrevPanel->Size.x) + PrevPanel->Position.x , (Position.y * PrevPanel->Size.y) + PrevPanel->Position.y   },Size,Core::RandomUInt64(0, std::numeric_limits<uint64_t>::max()) };
			m_PanelIDs[m_CurrentPanel].pParent = PrevPanel;

		}
		else {
			m_PanelIDs[m_CurrentPanel].Position = { Position.x ,Position.y };

		}
	}
	else {
		if (m_PanelDepth >= 1) {
			PanelData* PrevPanel = m_CurrenPanelParent;
			m_PanelIDs[m_CurrentPanel] = { { (m_PanelIDs[m_CurrentPanel].Position.x * PrevPanel->Size.x) + PrevPanel->Position.x , (m_PanelIDs[m_CurrentPanel].Position.y * PrevPanel->Size.y) + PrevPanel->Position.y   },m_PanelIDs[m_CurrentPanel].Size,Core::RandomUInt64(0, std::numeric_limits<uint64_t>::max()) };
			m_PanelIDs[m_CurrentPanel].pParent = PrevPanel;

		}
		else {
			m_PanelIDs[m_CurrentPanel].Position = { m_PanelIDs[m_CurrentPanel].Position.x ,m_PanelIDs[m_CurrentPanel].Position.y };

		}
	}
		


	m_CurrenPanelParent = &m_PanelIDs[m_CurrentPanel];

	if (Dragable) {
		if (inputSystem->IsMouseClicked(MouseCodes::LEFT, false))
			m_PanelIDs[m_CurrentPanel].ClickedPos = m_Application->GetMousePosNorm();


		if (inputSystem->IsMouseClicked(MouseCodes::LEFT,true))
		{
			uint64_t* id{};
			Buffer* buffer = renderer->GetCustomBuffer(0);
			Float2 data = buffer->ReadPixel((uint32_t)inputSystem->GetMousePos().x, (uint32_t)inputSystem->GetMousePos().y, renderer->GetViewPortExtent().width, renderer->GetViewPortExtent().height);
			Float2 Pos = m_Application->GetMousePosNorm();
			id = (uint64_t*)&data;
			if (m_PanelIDs[m_CurrentPanel].ID.ID == *id) {
				Float2 Change = { Pos.x - m_PanelIDs[m_CurrentPanel].ClickedPos.x,Pos.y - m_PanelIDs[m_CurrentPanel].ClickedPos.y };
				m_PanelIDs[m_CurrentPanel].Position.x += Change.x;
				m_PanelIDs[m_CurrentPanel].Position.y += Change.y;
				Core::Log(ErrorType::Info, "Change ", Change.x, "", Change.y);
				Core::Log(ErrorType::Info, "Position ", Pos.x, "", Pos.y);

			}
		}


	}
	renderer->DrawQuadGUI({ m_PanelIDs[m_CurrentPanel].Position.x,m_PanelIDs[m_CurrentPanel].Position.y,0.0f }, Color, Size,m_PanelIDs[m_CurrentPanel].ID.ID);
		m_PanelDepth++;
		m_CurrentPanel++;
	
}
bool GUIRenderer::Button(Float2 Position,Float4 Color,Float2 Size,MouseCodes mousecode,GUUID TextureHandle,bool SavesState, bool Dragable,bool** IsPressed)
{
	Renderer* renderer = m_Application->m_Renderer;
	InputSystem* inputsystem = &m_Application->m_InputSystem;
	Float2 LPosition{ Position };
	uint64_t* id{};

	if (m_CurrentButton == m_ButtonIDs.size())
		m_ButtonIDs[m_CurrentButton] = { false,Core::RandomUInt64(0, std::numeric_limits<uint64_t>::max()) };
	if (m_CurrenPanelParent) {
		LPosition = { (Position.x * m_CurrenPanelParent->Size.x) + m_CurrenPanelParent->Position.x,(Position.y * m_CurrenPanelParent->Size.y) + m_CurrenPanelParent->Position.y };

	}
	if(TextureHandle ==0)
		renderer->DrawQuadGUI({ LPosition.x,LPosition.y,0.0f }, Color, Size, m_ButtonIDs[m_CurrentButton].ID.ID);
	else
		renderer->DrawQuadGUI({ LPosition.x,LPosition.y,0.0f }, Color, Size, TextureHandle,m_ButtonIDs[m_CurrentButton].ID.ID );

	if(IsPressed)
		*IsPressed = &m_ButtonIDs[m_CurrentButton].IsPressed;

	if (inputsystem->IsMouseClicked(mousecode))
	{
		Buffer* buffer = renderer->GetCustomBuffer(0);
		Float2 data = buffer->ReadPixel((uint32_t)inputsystem->GetMousePos().x, (uint32_t)inputsystem->GetMousePos().y, renderer->GetViewPortExtent().width, renderer->GetViewPortExtent().height);
		Float2 Pos = inputsystem->GetMousePos();
		id = (uint64_t*)&data;
		if (m_ButtonIDs[m_CurrentButton].ID.ID == *id) {
			if (SavesState == false) {
				m_CurrentButton++;
				return true;
			}
			if (m_ButtonIDs[m_CurrentButton].IsPressed == true)
				m_ButtonIDs[m_CurrentButton].IsPressed = false;
			else
				m_ButtonIDs[m_CurrentButton].IsPressed= true;
		}
	}
	
	m_CurrentButton++;
	if (SavesState == false)
		return false;
	return m_ButtonIDs[m_CurrentButton-1].IsPressed;
}
void GUIRenderer::EndPanel()
{
	if (m_CurrenPanelParent->pParent)
		m_CurrenPanelParent = m_CurrenPanelParent->pParent;
	else
		m_CurrenPanelParent = nullptr;
	m_PanelDepth--;
}
void GUIRenderer::EndGUI()
{
	//delete button that hasn't been used.
}

