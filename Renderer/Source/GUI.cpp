#include "GUI.h"
#include "Renderer.h"
#include "Application.h"
#include "RandomGenerator.h"
#include "FontSystem.h"

GUIRenderer::GUIRenderer(Application* app,bool SaveState): m_Application(app),m_SaveState(SaveState)
{
	m_FontSystem = app->m_FontSystem;
}
void GUIRenderer::BeginGUI()
{
	m_CurrentPanel = 0;
}
void GUIRenderer::Panel(const std::string& ID,Float2 Position, Float4 Color, Float2 Size, GUUID TextureHandle, bool Dragable)
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
		if (inputSystem->IsMouseClicked(MouseCodes::LEFT, false)){
			m_PanelIDs[m_CurrentPanel].Offset.x = m_PanelIDs[m_CurrentPanel].Position.x-m_Application->GetMousePosNorm().x;
			m_PanelIDs[m_CurrentPanel].Offset.y =m_PanelIDs[m_CurrentPanel].Position.y -m_Application->GetMousePosNorm().y;
		}
		 if (m_DraggedPanel != -1&& !inputSystem->IsMouseClicked(MouseCodes::LEFT, true)) {
			 m_DraggedPanel = -1;
		}



		if (inputSystem->IsMouseClicked(MouseCodes::LEFT,true) &&m_DraggedPanel ==-1)
		{

			uint64_t* id{};
			Buffer* buffer = renderer->GetCustomBuffer(0);
			Float2 data = buffer->ReadPixel((uint32_t)inputSystem->GetMousePos().x, (uint32_t)inputSystem->GetMousePos().y, renderer->GetViewPortExtent().width, renderer->GetViewPortExtent().height);
			Float2 Pos = m_Application->GetMousePosNorm();
			id = (uint64_t*)&data;
			if (m_PanelIDs[m_CurrentPanel].ID.ID == *id) {
				Float2 Dis = { Pos.x-m_PanelIDs[m_CurrentPanel].Position.x,Pos.y-m_PanelIDs[m_CurrentPanel].Position.y };
				m_DraggedPanel = m_CurrentPanel;
				Dis.x += m_PanelIDs[m_CurrentPanel].Offset.x;
				Dis.y += m_PanelIDs[m_CurrentPanel].Offset.y;

				m_DraggedPanelDragAmount.x = Pos.x- m_PanelIDs[m_CurrentPanel].Offset.x;
				m_DraggedPanelDragAmount.y = Pos.y- m_PanelIDs[m_CurrentPanel].Offset.y;


			}
		}
		if (m_DraggedPanel != -1)
		{
			Float2 Pos = m_Application->GetMousePosNorm();
			Float2 Dis = { Pos.x - m_PanelIDs[m_DraggedPanel].Position.x,Pos.y - m_PanelIDs[m_DraggedPanel].Position.y };
			Dis.x += m_PanelIDs[m_DraggedPanel].Offset.x;
			Dis.y += m_PanelIDs[m_DraggedPanel].Offset.y;

			m_PanelIDs[m_DraggedPanel].Position.x += Pos.x- m_PanelIDs[m_DraggedPanel].Position.x+ m_PanelIDs[m_DraggedPanel].Offset.x;
			m_PanelIDs[m_DraggedPanel].Position.y += Pos.y - m_PanelIDs[m_DraggedPanel].Position.y + m_PanelIDs[m_DraggedPanel].Offset.y;

		}

	}
	if(TextureHandle !=0)
		renderer->DrawQuad({ m_PanelIDs[m_CurrentPanel].Position.x,m_PanelIDs[m_CurrentPanel].Position.y,0.0f }, Color, Size,TextureHandle, m_PanelIDs[m_CurrentPanel].ID.ID,0);
	else
		renderer->DrawQuad({ m_PanelIDs[m_CurrentPanel].Position.x,m_PanelIDs[m_CurrentPanel].Position.y,0.0f },Color, Size,m_PanelIDs[m_CurrentPanel].ID.ID);
		m_PanelDepth++;
		m_CurrentPanel++;
	
}
bool GUIRenderer::Button(const std::string& ID,const std::string& Text,Float2 Position,Float4 Color,Float2 Size,MouseCodes mousecode,GUUID TextureHandle,bool SavesState, bool Dragable,bool** IsPressed)
{
	Renderer* renderer = m_Application->m_Renderer;
	InputSystem* inputsystem = &m_Application->m_InputSystem;
	Float2 LPosition{ Position };
	uint64_t* id{};
	ButtonData* CurrentButtonData{};
	GUUID CurrentButtonID{};


	if (m_Buttons.find(ID) == m_Buttons.end())
		m_Buttons[ID] = { false };

	CurrentButtonData = &m_Buttons[ID];
	CurrentButtonID = Core::GetStringHash(ID);

	if (m_CurrenPanelParent) {
		LPosition = { (Position.x * m_CurrenPanelParent->Size.x) + m_CurrenPanelParent->Position.x,(Position.y * m_CurrenPanelParent->Size.y) + m_CurrenPanelParent->Position.y };

	}
	if(TextureHandle ==0)
		renderer->DrawQuad({ LPosition.x,LPosition.y,0.0f }, Color, Size, CurrentButtonID.ID);
	else
		renderer->DrawQuad({ LPosition.x,LPosition.y,0.0f }, Color, Size, TextureHandle, CurrentButtonID.ID,0 );

	//draw text
	if (Text.size() != 0) {
		m_FontSystem->Text(CurrentButtonID, Text.c_str(), { LPosition.x - Size.x ,LPosition.y - Size.y }, { Size.x*2,Size.y*2 });
	}


	if (IsPressed)
		Core::Log(ErrorType::Error, "Not implemented.");

	if (inputsystem->IsMouseClicked(mousecode))
	{
		Buffer* buffer = renderer->GetCustomBuffer(0);
		Float2 data = buffer->ReadPixel((uint32_t)inputsystem->GetMousePos().x, (uint32_t)inputsystem->GetMousePos().y, renderer->GetViewPortExtent().width, renderer->GetViewPortExtent().height);
		Float2 Pos = inputsystem->GetMousePos();
		id = (uint64_t*)&data;
		if (CurrentButtonID.ID == *id) {
			if (SavesState == false) {
				
				return true;

			}
			if (CurrentButtonData->IsPressed == true)
				CurrentButtonData->IsPressed = false;
			else
				CurrentButtonData->IsPressed= true;
		}
	}
	
	if (SavesState == false)
		return false;
	return CurrentButtonData->IsPressed;
}
void GUIRenderer::Text(const std::string& strID, const std::string& Text, Float2 Position, Float4 Color, Float2 Size) {
	if (Text.size() != 0) {
		m_FontSystem->Text(Core::GetStringHash(strID), Text.c_str(), Position, { Size.x * 2,Size.y * 2 });
	}
}
void GUIRenderer::Slider(const std::string& strID, float* number, Float2 Position, Float2 Size, float SlideAmount, uint32_t DecimalPlaces)
{
	std::string StringNumber = std::to_string(*number);
	SliderData* CurrentSlider = &m_Sliders[strID];


	if (Button(strID, StringNumber.substr(0, StringNumber.size() - (6 - DecimalPlaces)), Position, { 1.0f,1.0f,1.0f,1.0f }, Size, MouseCodes::LEFT, 0, false)) {
		if (!CurrentSlider->IsClicked) {
			CurrentSlider->IsClicked = true;
		}
	}


	if (CurrentSlider->IsClicked == true && m_Application->m_InputSystem.IsMouseClicked(MouseCodes::LEFT, true)) {
		*number += m_Application->m_InputSystem.GetMousePosChange().x * SlideAmount;

	}
	else {
		CurrentSlider->IsClicked = false;

	}

}
void GUIRenderer::InputText(const char* ID,char* Buffer,uint64_t BufferSize,Float2 Position,Float2 Size) {
	m_FontSystem->InputText(ID, Buffer, BufferSize, Position, Size);
}
void GUIRenderer::EndPanel()
{
	

	if (m_CurrenPanelParent->pParent)
		m_CurrenPanelParent = m_CurrenPanelParent->pParent;
	else
		m_CurrenPanelParent = nullptr;
	m_PanelDepth--;
}
void GUIRenderer::SetFontSize(float Size)
{
	m_FontSystem->SetCharcterSize(Size);
}
float GUIRenderer::GetFontSize() {
	return m_FontSystem->GetFontSize().x;
}

void GUIRenderer::EndGUI()
{
	//Update the dragged panel/button


	//delete button that hasn't been used.
}

