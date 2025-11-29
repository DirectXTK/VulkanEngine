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

	GUI::OutlineStyle OutlineStyle{sizeof(GUI::OutlineStyle)};
	OutlineStyle.Color = {1.0f,1.0f,0.0f,1.0f};
	OutlineStyle.Width =0.01f;

	PushStyle(GUI::Style::COLOR, &ColorData);
	PushStyle(GUI::Style::BORDER,&BorderData);
	PushStyle(GUI::Style::OUTLINE,&OutlineStyle);

	Renderer* renderer = Application::GetRenderer();
	
}
void GUIRenderer::BeginGUI()
{
	m_CurrentPanel = 0;

	Renderer* renderer =Application::GetRenderer(); 
	Buffer* buffer = renderer->GetCustomBuffer(0);
	delete []m_PickBufferData;
	m_PickBufferData = new Float2[renderer->GetViewPortExtent().width*renderer->GetViewPortExtent().height];
	buffer->LoadFromBufferToVar(m_PickBufferData,renderer->GetViewPortExtent().width*renderer->GetViewPortExtent().height*sizeof(Float2));
	//Core::Log("Size",renderer->GetViewPortExtent().height," ", renderer->GetViewPortExtent().width);
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
			Float2 data{};
			bool succeded = Core::ReadPixel(m_PickBufferData,renderer->GetViewPortExtent().width,renderer->GetViewPortExtent().height,Application::GetMousePos().x,Application::GetMousePos().y,&data);
			if(succeded){
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
	Float2 OutlineSize{Size.x+m_CurrentOutlineData->Width,Size.y+m_CurrentOutlineData->Width};
	if(CurrentButtonData->IsPressed == true)
	renderer->DrawQuad({LPosition.x,LPosition.y,0.0f},m_CurrentOutlineData->Color,OutlineSize,0);



	if(TextureHandle ==0)
		renderer->DrawQuad({ LPosition.x,LPosition.y,0.0f }, Color, Size, CurrentButtonID.ID);
	else
		renderer->DrawQuad({ LPosition.x,LPosition.y,0.0f }, Color, Size, TextureHandle, CurrentButtonID.ID,0 );

	//draw text
	if (Text.size() != 0) {
		m_FontSystem->Text(CurrentButtonID, Text.c_str(), { LPosition.x  ,LPosition.y }, { Size.x,Size.y });
	}

	


	if (inputsystem->IsMouseClicked(mousecode)  )
	{	
		Float2 data{};
		bool succeded = Core::ReadPixel(m_PickBufferData,renderer->GetViewPortExtent().width,renderer->GetViewPortExtent().height,Application::GetMousePos().x,Application::GetMousePos().y,&data);
		if(succeded){
		id = (uint64_t*)&data;
		CurrentButtonData->LastClicked = Time::GetTimeNs();

		if (CurrentButtonID.ID == *id) {

			if (SavesState )
			{
				if(CurrentButtonData->IsPressed==true)
					CurrentButtonData->IsPressed=false;
				else
					CurrentButtonData->IsPressed=true;

				return CurrentButtonData->IsPressed;
			}
			else
			CurrentButtonData->IsPressed=true;
				
		}else{
			if(!SavesState)
				CurrentButtonData->IsPressed=false;

		}
	}
	
	}
	


	return CurrentButtonData->IsPressed;
}
void GUIRenderer::Text(const std::string& strID, const std::string& Text, Float2 Position, Float4 Color, Float2 Size) {
	if (Text.size() != 0) {
		
	

		if (m_CurrenPanelParent) {
			Size = {Size.x*m_CurrenPanelParent->Size.x,Size.y*m_CurrenPanelParent->Size.y};
		Position = {m_CurrenPanelParent->Size.x*Position.x+m_CurrenPanelParent->Position.x,m_CurrenPanelParent->Size.y*Position.y+m_CurrenPanelParent->Position.y};
		}
		DrawBorder(Position,Size,m_CurrentBorderData->BorderColor,m_CurrentBorderData->BackGroundColor,m_CurrentBorderData->BorderWidth);
		m_FontSystem->Text(Core::GetStringHash(strID), Text.c_str(), Position, { Size.x ,Size.y  });
	}
}
void GUIRenderer::DrawBorder(const Float2& Position, const Float2& Size, const Float4& BorderColor,const Float4& BackGroundColor, float BorderWidth) {
	Renderer* renderer = m_Application->m_Renderer;
	Float2 BorderSize{ Size.x+ BorderWidth,Size.y+ BorderWidth };
	Float2 RealPosition= {Position.x,Position.y};
	
	renderer->DrawQuad({ RealPosition.x,RealPosition.y,0.0f }, BorderColor, BorderSize, 0);
	renderer->DrawQuad({ RealPosition.x,RealPosition.y,0.0f }, BackGroundColor, Size, 0);
}
void GUIRenderer::Slider(const std::string& strID, float* number, Float2 Position, Float2 Size, float SlideAmount,Float2 MinMax, uint32_t DecimalPlaces)
{
	Float4 Color{m_CurrentColor};
	Renderer* renderer= m_Application->m_Renderer;
	std::string StringNumber = std::to_string(*number);
	SliderData* CurrentSlider = &m_Sliders[strID];
	 float sliderClickedColorMin{.2f};

	
	if (CurrentSlider->IsClicked == true)
		sliderClickedColorMin=0.2f;
	else
		sliderClickedColorMin=0;

	Color = { Color.r - sliderClickedColorMin,Color.g - sliderClickedColorMin,Color.b - sliderClickedColorMin,m_CurrentColor.a};



	if(m_CurrentBorderData){
		if (m_CurrentBorderData->DrawBorder){
				Float4 BackGroundColor = {m_CurrentBorderData->BackGroundColor.r-sliderClickedColorMin,m_CurrentBorderData->BackGroundColor.g-sliderClickedColorMin,m_CurrentBorderData->BackGroundColor.b-sliderClickedColorMin,m_CurrentBorderData->BackGroundColor.a};
			DrawBorder(Position, Size, m_CurrentBorderData->BorderColor,BackGroundColor, m_CurrentBorderData->BorderWidth);
		}
	}

	if(m_CurrentSliderData){
		if(m_CurrentSliderData->FillOn)
		{
			Color.a =0;
			float procent = *number/MinMax.y;
			renderer->DrawQuad({Position.x-((1-procent)*Size.x),Position.y},m_CurrentSliderData->FillColor,{Size.x*procent,Size.y},Core::GetStringHash(strID).ID);
		}
	}


	if (Button(strID, StringNumber.substr(0, StringNumber.size() - (6 - DecimalPlaces)), Position, { Color }, Size, MouseCodes::LEFT, 0, false)) {
		if (!CurrentSlider->IsClicked) {
			CurrentSlider->IsClicked = true;

		}
	}

	


	
	
	if (CurrentSlider->IsClicked == true && m_Application->m_InputSystem.IsMouseClicked(MouseCodes::LEFT, true)) {
		*number -= m_Application->m_InputSystem.GetMousePosChange().x * SlideAmount;
		*number=std::clamp(*number,MinMax.x,MinMax.y);
	}
	else {
		CurrentSlider->IsClicked = false;

	}

}
void GUIRenderer::Slider(const std::string& strID, int* number, Float2 Position, Float2 Size, float SlideAmount )
{
	Float4 Color{m_CurrentColor};
	std::string StringNumber = std::to_string(*number);
	SliderData* CurrentSlider = &m_Sliders[strID];

	if (m_CurrentBorderData->DrawBorder)
		DrawBorder(Position, Size, m_CurrentBorderData->BorderColor,m_CurrentBorderData->BackGroundColor, m_CurrentBorderData->BorderWidth);

	if (CurrentSlider->IsClicked == true)
		Color = { Color.r - 0.15f,Color.g - 0.15f,Color.b - 0.15f};

	if (Button(strID, StringNumber.substr(0, StringNumber.size() - (6 - 2)), Position, { Color }, Size, MouseCodes::LEFT,0,false)) {
		if (!CurrentSlider->IsClicked) {
			CurrentSlider->IsClicked = true;
		Core::Log(ErrorType::Error,"Lafa");

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
		Color = (GUI::ColorStyle*)malloc(sizeof(GUI::ColorStyle));
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
		OutputBorder = (GUI::BorderStyle*)malloc(sizeof(GUI::BorderStyle));
		*OutputBorder = *TransformedData;

		m_Styles.push({ style,OutputBorder });

		break;
	}
	case GUI::Style::SLIDER: {
		GUI::SliderStyle* TransformedData = (GUI::SliderStyle*)Data;
		GUI::SliderStyle* sliderData{};

		if (TransformedData->StructSize != sizeof(GUI::SliderStyle))
		{
			Core::Log(ErrorType::Error, "Invalid Style or size is specified incorectly.");
			return;
		}
		sliderData = (GUI::SliderStyle*)malloc(sizeof(GUI::SliderStyle));
		*sliderData = *TransformedData;

		m_Styles.push({ style,sliderData });

		break;
	}
	case GUI::Style::OUTLINE: {
		GUI::OutlineStyle* TransformedData = (GUI::OutlineStyle*)Data;
		GUI::OutlineStyle* sliderData{};

		if (TransformedData->StructSize != sizeof(GUI::OutlineStyle))
		{
			Core::Log(ErrorType::Error, "Invalid Style or size is specified incorectly.");
			return;
		}
		sliderData = (GUI::OutlineStyle*)malloc(sizeof(GUI::OutlineStyle));
		*sliderData = *TransformedData;

		m_Styles.push({ style,sliderData });

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
	free(container.StyleData);

	m_CurrentBorderData= nullptr;
	m_CurrentSliderData= nullptr;

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
	case GUI::Style::SLIDER: {
		GUI::SliderStyle* slider = (GUI::SliderStyle*)Container.StyleData;
		m_CurrentSliderData = slider;
		break;
	}
	case GUI::Style::OUTLINE: {
		GUI::OutlineStyle* outline = (GUI::OutlineStyle*)Container.StyleData;
		m_CurrentOutlineData = outline;
		break;
	}
	case GUI::Style::NONE: {

	}
	default: {
		Core::Log(ErrorType::Error,"Reapply styles style in invalid ",(uint32_t)Container.StyleType);
	}
	}
}
void GUIRenderer::SetFontSize(uint32_t Size)
{	
	if(Size >0)
		m_FontSystem->SetCharcterSize(Size);
}
uint32_t GUIRenderer::GetFontSize() {
	return m_FontSystem->GetFontSize();
}

void GUIRenderer::EndGUI()
{
	//Update the dragged panel/button


	//delete button that hasn't been used.
}
	GUIRenderer::~GUIRenderer(){
		delete[] m_PickBufferData;
	}

