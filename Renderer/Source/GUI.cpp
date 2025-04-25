#include "GUI.h"
#include "Renderer.h"
#include "Application.h"
#include "RandomGenerator.h"
#include "FontSystem.h"

GUIRenderer::GUIRenderer(Application* app,bool SaveState): m_Application(app),m_SaveState(SaveState)
{
	m_FontSystem = app->m_FontSystem;
	//Push all the default styles
	GUI::ColorStyle ColorData{sizeof(GUI::ColorStyle)};
	ColorData.Color = { 1.0f,1.0f,1.0f,1.0f };

	GUI::BorderStyle BorderData{ sizeof(GUI::BorderStyle) };
	BorderData.DrawBorder = false;

	PushStyle(GUI::Style::COLOR, &ColorData);
	PushStyle(GUI::Style::BORDER,&BorderData);

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
void GUIRenderer::DrawBorder(const Float2& Position, const Float2& Size, const Float4& BorderColor, float BorderWidth) {
	Renderer* renderer = m_Application->m_Renderer;
	Float2 BorderSize{ Size.x+ BorderWidth,Size.y+ BorderWidth };
	
	
	renderer->DrawQuad({ Position.x,Position.y,0.0f }, BorderColor, BorderSize, 0);
}
void GUIRenderer::Slider(const std::string& strID, float* number, Float2 Position, Float2 Size, float SlideAmount, uint32_t DecimalPlaces)
{
	Float4 Color{m_CurrentColor};
	std::string StringNumber = std::to_string(*number);
	SliderData* CurrentSlider = &m_Sliders[strID];

	if (m_CurrentBorderData->DrawBorder)
		DrawBorder(Position, Size, m_CurrentBorderData->BorderColor, m_CurrentBorderData->BorderWidth);

	if (CurrentSlider->IsClicked == true)
		Color = { Color.r - 0.15f,Color.g - 0.15f,Color.b - 0.15f,1.0f };

	if (Button(strID, StringNumber.substr(0, StringNumber.size() - (6 - DecimalPlaces)), Position, { Color }, Size, MouseCodes::LEFT, 0, false)) {
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
void GUIRenderer::PushStyle(const GUI::Style& style, void* Data)
{
	switch (style) {
	case GUI::Style::COLOR: {
		GUI::ColorStyle* Color{};
		GUI::ColorStyle* TransformedData = (GUI::ColorStyle*)Data;

		if (TransformedData->StructSize != sizeof(GUI::ColorStyle))
		{
			Core::Log(ErrorType::Error, "Invalid Style or size is specified incorectly.");
			return;
		}
		Color = new GUI::ColorStyle();
		*Color = *TransformedData;


		m_Styles.push({ style,Color });
		break;
	}
	case GUI::Style::BORDER: {
		GUI::BorderStyle* TransformedData = (GUI::BorderStyle*)Data;
		GUI::BorderStyle* OutputBorder{};

		if (TransformedData->StructSize != sizeof(GUI::BorderStyle))
		{
			Core::Log(ErrorType::Error, "Invalid Style or size is specified incorectly.");
			return;
		}
		OutputBorder = new GUI::BorderStyle();
		*OutputBorder = *TransformedData;

		m_Styles.push({ GUI::Style::BORDER,OutputBorder });

		break;
	}
	case GUI::Style::NONE: {
		Core::Log(ErrorType::Error, "GUI style is not specified.");
		break;
	}
	default: {
		Core::Log(ErrorType::Error, "Invalid GUI style");
		break;
	}
	}
	ReapplyStyles();
}
void GUIRenderer::PopStyle() {
	StyleContainer container = m_Styles.top();
	delete container.StyleData;

	m_Styles.pop();
	ReapplyStyles();

}
void GUIRenderer::ReapplyStyles() {
	StyleContainer Container = m_Styles.top();

	switch (Container.StyleType) {
	case GUI::Style::COLOR: {
		GUI::ColorStyle* ColorData = (GUI::ColorStyle*)Container.StyleData;

		m_CurrentColor = ColorData->Color;

		break;
	}
	case GUI::Style::BORDER: {
		GUI::BorderStyle* Border = (GUI::BorderStyle*)Container.StyleData;

		m_CurrentBorderData = Border;
		break;
	}
	case GUI::Style::NONE: {

	}
	default: {

	}
	}
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

