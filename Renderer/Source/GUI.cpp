#include "GUI.h"
#include "Renderer.h"
#include "Application.h"
#include "RandomGenerator.h"
#include "FontSystem.h"

struct TooltipData{
	TooltipData(const std::string& str): tooltip(str) {}
	std::string tooltip{};
	Float2 MaxSize{};
	Float2 PosOffset{};
	GUUID ID{0};
	Float4 BorderColor{1.0f,1.0f,1.0f,1.0f};
	Float4 BackGroundColor{1.0f,1.0f,1.0f,0.0f};
};
		
GUIRenderer::GUIRenderer(Application* app,bool SaveState): m_Application(app),m_SaveState(SaveState)
{
	m_StylesInQueue.resize((uint32_t)(GUI::Style::NUMOFSTYLES)-1);

	m_FontSystem = app->m_FontSystem;
	//Push all the default styles



}
void GUIRenderer::BeginGUI()
{
	
	Renderer* renderer =Application::GetRenderer(); 
	Buffer* buffer = renderer->GetPickingBuffer(Application::GetRender()->GetRenderedFrame());
	delete []m_PickBufferData;
	m_PickBufferData = new Float2[renderer->GetViewPortExtent().width*renderer->GetViewPortExtent().height];
	buffer->LoadFromBufferToVar(m_PickBufferData,renderer->GetViewPortExtent().width*renderer->GetViewPortExtent().height*sizeof(Float2));
}


void GUIRenderer::Panel(const std::string& ID,Float2 Position, Float4 Color, Float2 Size, GUUID TextureHandle, bool Dragable)
{
	InputSystem* inputSystem = &m_Application->m_InputSystem;
	Render* renderer = m_Application->GetRender();
	GUUID id(ID);
	
	if (m_PanelIDs.find(id) == m_PanelIDs.end()) {
		m_PanelIDs[id] = { Position,Size,id };
	}

	if (!Dragable) {
		if (m_PanelDepth >= 1) {
			PanelData* PrevPanel = m_CurrenPanelParent;
			m_PanelIDs[id] = { { (Position.x * PrevPanel->Size.x) + PrevPanel->Position.x , (Position.y * PrevPanel->Size.y) + PrevPanel->Position.y   },Size,Core::RandomUInt64(0, std::numeric_limits<uint64_t>::max()) };
			m_PanelIDs[id].pParent = PrevPanel;

		}
	
	}
	else {
		if (m_PanelDepth >= 1) {
			PanelData* PrevPanel = m_CurrenPanelParent;
			m_PanelIDs[id] = { { (m_PanelIDs[id].Position.x * PrevPanel->Size.x) + PrevPanel->Position.x , (m_PanelIDs[id].Position.y * PrevPanel->Size.y) + PrevPanel->Position.y   },m_PanelIDs[id].Size,Core::RandomUInt64(0, std::numeric_limits<uint64_t>::max()) };
			m_PanelIDs[id].pParent = PrevPanel;
		}
	
	}
		


	m_CurrenPanelParent = &m_PanelIDs[id];

	if (Dragable) {
		if (inputSystem->IsMouseClicked(MouseCodes::LEFT, false)){
			m_PanelIDs[id].Offset.x = m_PanelIDs[id].Position.x-m_Application->GetMousePosNorm().x;
			m_PanelIDs[id].Offset.y =m_PanelIDs[id].Position.y -m_Application->GetMousePosNorm().y;
		}
		
		
		if (m_DraggedPanel == id) 
		{	
			Float2 Pos = m_Application->GetMousePosNorm();
			Float2 Dis = { Pos.x - m_PanelIDs[m_DraggedPanel].Position.x,Pos.y - m_PanelIDs[m_DraggedPanel].Position.y };
			Dis.x += m_PanelIDs[m_DraggedPanel].Offset.x;
			Dis.y += m_PanelIDs[m_DraggedPanel].Offset.y;

			m_PanelIDs[m_DraggedPanel].Position.x += Pos.x- m_PanelIDs[m_DraggedPanel].Position.x+ m_PanelIDs[m_DraggedPanel].Offset.x;
			m_PanelIDs[m_DraggedPanel].Position.y += Pos.y - m_PanelIDs[m_DraggedPanel].Position.y + m_PanelIDs[m_DraggedPanel].Offset.y;

		}
	}
	ApplyCurrentStyles(m_PanelIDs[id].Position,Size,Color,ID);

	if(TextureHandle !=0)
		renderer->DrawQuad({ m_PanelIDs[id].Position.x,m_PanelIDs[id].Position.y }, Color, Size,TextureHandle, m_PanelIDs[id].ID.ID,0);
	else
		renderer->DrawQuad({ m_PanelIDs[id].Position.x,m_PanelIDs[id].Position.y },Color, Size,m_PanelIDs[id].ID);
	m_PanelDepth++;
}
void GUIRenderer::Tooltip(const std::string& tooltip,const Float2& maxSize,const Float2& posOffset,const Float4& backGroundColor,const Float4& borderColor,GUUID id){
	TooltipData* data = new TooltipData(tooltip);
	data->MaxSize = maxSize;
	data->PosOffset = posOffset;
	data->BackGroundColor = backGroundColor;
	data->BorderColor = borderColor;
	data->ID = id;

	m_StylesInQueue[(uint32_t)GUI::Style::TOOLTIP-1].push({GUI::Style::TOOLTIP,data});
}
bool GUIRenderer::Button(const std::string& ID,const std::string& Text,Float2 Position,Float4 Color,Float2 Size,MouseCodes mousecode,GUUID TextureHandle,bool SavesState, bool Dragable,bool** IsPressed)
{
	Render* renderer = m_Application->GetRender();
	InputSystem* inputsystem = &m_Application->m_InputSystem;
	Float2 LPosition{ Position };
	ButtonData* CurrentButtonData{};
	GUUID CurrentButtonID{};


	if (m_Buttons.find(ID) == m_Buttons.end())
		m_Buttons[ID] = { false };

	CurrentButtonData = &m_Buttons[ID];
	CurrentButtonID = Core::GetStringHash(ID);

	if (m_CurrenPanelParent) {
		LPosition = { (Position.x * m_CurrenPanelParent->Size.x) + m_CurrenPanelParent->Position.x,(Position.y * m_CurrenPanelParent->Size.y) + m_CurrenPanelParent->Position.y };
		
		Size = {Size.x*m_CurrenPanelParent->Size.x,Size.y*m_CurrenPanelParent->Size.y};
		Size.x = std::clamp(Size.x,0.0f,m_CurrenPanelParent->Size.x);
		Size.y = std::clamp(Size.y,0.0f,m_CurrenPanelParent->Size.y);

		LPosition.x = std::clamp(LPosition.x,m_CurrenPanelParent->Position.x-m_CurrenPanelParent->Size.x+Size.x,m_CurrenPanelParent->Position.x+m_CurrenPanelParent->Size.x-Size.x);
		LPosition.y = std::clamp(LPosition.y,m_CurrenPanelParent->Position.y-m_CurrenPanelParent->Size.y+Size.y,m_CurrenPanelParent->Position.y+m_CurrenPanelParent->Size.y-Size.y);
	}
	ApplyCurrentStyles(LPosition,Size,Color,CurrentButtonID);

	if(CurrentButtonData->IsPressed == true){
		//if(SavesState)
		//	renderer->DrawQuad({LPosition.x,LPosition.y,0.0f},m_CurrentOutlineData->Color,OutlineSize,0);
		//else{
		//	Color.r -=0.1f;
		//	Color.g -=0.1f;
		//	Color.b -=0.1f;
		//}
	}



	if(TextureHandle ==0)
		renderer->DrawQuad({ LPosition.x,LPosition.y}, Color, Size, CurrentButtonID.ID);
	else
		renderer->DrawQuad({ LPosition.x,LPosition.y }, Color, Size,  CurrentButtonID.ID,TextureHandle,0 );

	//draw text
	if (Text.size() != 0) {
		m_FontSystem->Text(CurrentButtonID, Text.c_str(), { LPosition.x  ,LPosition.y }, { Size.x,Size.y });
	}

	

		if (m_SelectedObjID == CurrentButtonID) {
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
	
	
	
	


	return CurrentButtonData->IsPressed;
}
void GUIRenderer::Quad(const Float2& position,const Float2& size,const Float4& color,GUUID textureID){

	Render* renderer = m_Application->GetRender();
	Float2 LPosition{ position };
	Float2 RSize{size};
	ButtonData* CurrentButtonData{};
	Float4 rColor{color};
	GUUID CurrentButtonID{};

	if (m_CurrenPanelParent) {
		LPosition = { (position.x * m_CurrenPanelParent->Size.x) + m_CurrenPanelParent->Position.x,(position.y * m_CurrenPanelParent->Size.y) + m_CurrenPanelParent->Position.y };
		
		RSize = {RSize.x*m_CurrenPanelParent->Size.x,size.y*m_CurrenPanelParent->Size.y};
		RSize.x = std::clamp(RSize.x,0.0f,m_CurrenPanelParent->Size.x);
		RSize.y = std::clamp(RSize.y,0.0f,m_CurrenPanelParent->Size.y);

		LPosition.x = std::clamp(LPosition.x,m_CurrenPanelParent->Position.x-m_CurrenPanelParent->Size.x-RSize.x,m_CurrenPanelParent->Position.x+m_CurrenPanelParent->Size.x-RSize.x);
		LPosition.y = std::clamp(LPosition.y,m_CurrenPanelParent->Position.y-m_CurrenPanelParent->Size.y-RSize.y,m_CurrenPanelParent->Position.y+m_CurrenPanelParent->Size.y-RSize.y);
	}
	ApplyCurrentStyles(LPosition,RSize,rColor);
	renderer->DrawQuad({LPosition.x,LPosition.y},rColor,RSize,0,textureID,-1);
}

bool GUIRenderer::CheckBox(const std::string& id,const Float2& position,const Float2& size,const Float4& color,GUUID customCheckBoxTexture){
	Render* renderer = m_Application->GetRender();
	Float2 LPosition{ position };
	Float2 RSize{size};
	CheckBoxData* currentCheckBoxData{};
	GUUID CurrentButtonID{Core::GetStringHash(id)};
	Float4 rColor = color;

	if (m_CheckBoxes.find(id) == m_CheckBoxes.end())
		m_CheckBoxes[id] = { false };

	currentCheckBoxData = &m_CheckBoxes[id];
	CurrentButtonID = Core::GetStringHash(id);

	if(m_SelectedObjID == CurrentButtonID){
		if(currentCheckBoxData->IsClicked)
			currentCheckBoxData->IsClicked = false;
		else 
			currentCheckBoxData->IsClicked = true;
	}

	if (m_CurrenPanelParent) {
		LPosition = { (position.x * m_CurrenPanelParent->Size.x) + m_CurrenPanelParent->Position.x,(position.y * m_CurrenPanelParent->Size.y) + m_CurrenPanelParent->Position.y };
		
		RSize = {RSize.x*m_CurrenPanelParent->Size.x,size.y*m_CurrenPanelParent->Size.y};
		RSize.x = std::clamp(RSize.x,0.0f,m_CurrenPanelParent->Size.x);
		RSize.y = std::clamp(RSize.y,0.0f,m_CurrenPanelParent->Size.y);

		LPosition.x = std::clamp(LPosition.x,m_CurrenPanelParent->Position.x-m_CurrenPanelParent->Size.x+RSize.x,m_CurrenPanelParent->Position.x+m_CurrenPanelParent->Size.x-RSize.x);
		LPosition.y = std::clamp(LPosition.y,m_CurrenPanelParent->Position.y-m_CurrenPanelParent->Size.y+RSize.y,m_CurrenPanelParent->Position.y+m_CurrenPanelParent->Size.y-RSize.y);
	}
	ApplyCurrentStyles(LPosition,RSize,rColor,CurrentButtonID);
	if(currentCheckBoxData->IsClicked){
		renderer->DrawQuad({LPosition.x,LPosition.y},color,RSize,CurrentButtonID.ID,Core::GetStringHash("GUI/CheckBoxTrue"),-1);
	}
	else{
		renderer->DrawQuad({LPosition.x,LPosition.y},color,RSize,CurrentButtonID.ID,Core::GetStringHash("GUI/CheckBoxFalse"),-1);
	}
	return currentCheckBoxData->IsClicked;
}
void GUIRenderer::Text(const std::string& strID, const std::string& Text, Float2 Position, Float4 Color, Float2 Size) {
	if (Text.size() != 0) {
		
	

		if (m_CurrenPanelParent) {
			Size = {Size.x*m_CurrenPanelParent->Size.x,Size.y*m_CurrenPanelParent->Size.y};
		Position = {m_CurrenPanelParent->Size.x*Position.x+m_CurrenPanelParent->Position.x,m_CurrenPanelParent->Size.y*Position.y+m_CurrenPanelParent->Position.y};
		}
		ApplyCurrentStyles(Position,Size,Color);
		m_FontSystem->Text(Core::GetStringHash(strID), Text.c_str(), Position, { Size.x ,Size.y  });
	}
}
void GUIRenderer::DrawBorder(const Float2& Position, const Float2& Size, const Float4& BorderColor,const Float4& BackGroundColor, float BorderWidth) {
	Render* renderer = m_Application->GetRender();
	Float2 BorderSize{ Size.x+ BorderWidth,Size.y+ BorderWidth };
	Float2 RealPosition= {Position.x,Position.y};
	
	renderer->DrawQuad({ RealPosition.x,RealPosition.y }, BorderColor, BorderSize, 0);
	renderer->DrawQuad({ RealPosition.x,RealPosition.y }, BackGroundColor, Size, 0);
}
void GUIRenderer::Slider(const std::string& strID,  float* number,const  Float2& Position,const Float4& color,const  Float2& Size,const  float SlideAmount,const Float2& MinMax,const  uint32_t DecimalPlaces)
{
	Float4 Color{color};
	Render* renderer= m_Application->GetRender();
	std::string StringNumber = std::to_string(*number);
	SliderData* CurrentSlider = &m_Sliders[strID];
	 float sliderClickedColorMin{.2f};
	Float2 lPosition{Position};
	Float2 rSize{Size};

	
	if (CurrentSlider->IsClicked == true)
		sliderClickedColorMin=0.2f;
	else
		sliderClickedColorMin=0;

	Color = { Color.r - sliderClickedColorMin,Color.g - sliderClickedColorMin,Color.b - sliderClickedColorMin,Color.a};

	if (m_CurrenPanelParent) {
		lPosition = { (Position.x * m_CurrenPanelParent->Size.x) + m_CurrenPanelParent->Position.x,(Position.y * m_CurrenPanelParent->Size.y) + m_CurrenPanelParent->Position.y };
		
		rSize = {rSize.x*m_CurrenPanelParent->Size.x,Size.y*m_CurrenPanelParent->Size.y};
		rSize.x = std::clamp(rSize.x,0.0f,m_CurrenPanelParent->Size.x);
		rSize.y = std::clamp(rSize.y,0.0f,m_CurrenPanelParent->Size.y);

		lPosition.x = std::clamp(lPosition.x,m_CurrenPanelParent->Position.x-m_CurrenPanelParent->Size.x-rSize.x,m_CurrenPanelParent->Position.x+m_CurrenPanelParent->Size.x+rSize.x);
		lPosition.y = std::clamp(lPosition.y,m_CurrenPanelParent->Position.y-m_CurrenPanelParent->Size.y-rSize.y,m_CurrenPanelParent->Position.y+m_CurrenPanelParent->Size.y+rSize.y);
	}

	ApplyCurrentStyles(lPosition,rSize,Color);


	if (Button(strID, StringNumber.substr(0, StringNumber.size() - (6 - DecimalPlaces)), Position, { Color }, Size, MouseCodes::LEFT, 0, false)) {
		if (!CurrentSlider->IsClicked) {
			CurrentSlider->IsClicked = true;
			CurrentSlider->MousePosChange = Application::GetMousePos().x;
		}
	}

	


	
	if (CurrentSlider->IsClicked == true && m_Application->m_InputSystem.IsMouseClicked(MouseCodes::LEFT, true)) {
		float deltaPos = Application::GetMousePos().x -CurrentSlider->MousePosChange;
		CurrentSlider->MousePosChange =  Application::GetMousePos().x ;
		*number += deltaPos * SlideAmount;
		*number=std::clamp(*number,MinMax.x,MinMax.y);
	}
	else {
		CurrentSlider->IsClicked = false;

	}

}
void GUIRenderer::Slider(const std::string& strID, int* number,const  Float2& Position,const Float4& color ,const Float2& Size,const  float SlideAmount )
{
	Float4 Color{color};

	std::string StringNumber = std::to_string(*number);
	SliderData* CurrentSlider = &m_Sliders[strID];
	Core::Log(ErrorType::Error,"Not implemented yet Slider(INT)");
	return;
	if (CurrentSlider->IsClicked == true)
		Color = { Color.r - 0.15f,Color.g - 0.15f,Color.b - 0.15f};

	if (Button(strID, StringNumber.substr(0, StringNumber.size() - (6 - 2)), Position, { Color }, Size, MouseCodes::LEFT,0,false)) {
		if (!CurrentSlider->IsClicked) {
			CurrentSlider->IsClicked = true;

		}
	}
		Core::Log(ErrorType::Error,"Lafa");

	
	if (CurrentSlider->IsClicked == true && m_Application->m_InputSystem.IsMouseClicked(MouseCodes::LEFT, true)) {
		*number += m_Application->m_InputSystem.GetMousePosChange().x * SlideAmount;
	}
	else {
		CurrentSlider->IsClicked = false;

	}

}
void GUIRenderer::UpdateLineStarts(std::vector<uint64_t>& lineStarts,const char* buffer,uint64_t bufferSize,float lineSizeX){
	lineStarts.clear();
	lineStarts.push_back(0);
	for(uint32_t i=0 ;i < bufferSize;i++){
		if(buffer[i] =='\0')
			break;
		if(buffer[i] == '\n')
			lineStarts.push_back(i+1);
	}
}
void GUIRenderer::InputText(const char* ID,char* Buffer,uint64_t BufferSize,Float2 Position,Float2 Size,bool scrollable) {
	Float2 rSize{Size};
	Float2 lPosition{Position};
	GUUID id = Core::GetStringHash(ID);
	Render* renderer  = Application::GetRender();
	Float4 color{};
	Float4 sliderBarColor{0.5f,0.5,0.5f,0.9f};
	Float4 sliderButtonColor{0.35f,0.4,0.4f,0.9f};

	if (m_CurrenPanelParent) {
		lPosition = { (Position.x * m_CurrenPanelParent->Size.x) + m_CurrenPanelParent->Position.x,(Position.y * m_CurrenPanelParent->Size.y) + m_CurrenPanelParent->Position.y };
		
		rSize = {rSize.x*m_CurrenPanelParent->Size.x,Size.y*m_CurrenPanelParent->Size.y};
		rSize.x = std::clamp(rSize.x,0.0f,m_CurrenPanelParent->Size.x);
		rSize.y = std::clamp(rSize.y,0.0f,m_CurrenPanelParent->Size.y);

		lPosition.x = std::clamp(lPosition.x,m_CurrenPanelParent->Position.x-m_CurrenPanelParent->Size.x-rSize.x,m_CurrenPanelParent->Position.x+m_CurrenPanelParent->Size.x+rSize.x);
		lPosition.y = std::clamp(lPosition.y,m_CurrenPanelParent->Position.y-m_CurrenPanelParent->Size.y-rSize.y,m_CurrenPanelParent->Position.y+m_CurrenPanelParent->Size.y+rSize.y);
	}
	ApplyCurrentStyles(lPosition,rSize,color,id);
	if(scrollable){
		

		float totalLogicalPosYSize{rSize.y};
		float logicalPosY{};

		auto it = m_InputTextData.find(id);
		if(it == m_InputTextData.end())
		{
			m_InputTextData[id] ={0};
			UpdateLineStarts(m_InputTextData[id].LineStarts,Buffer,BufferSize,0);
		}else{
			if(m_CurrentlySelectedObject == Core::GetStringHash(ID)){
				it->second.ScrollYIndex -= m_Scroll;
				
			}
			it->second.ScrollYIndex = std::clamp(it->second.ScrollYIndex ,(int64_t)0,(int64_t)m_InputTextData[id].LineStarts.size()-1);
			totalLogicalPosYSize =m_InputTextData[id].LineStarts.size();
			if(m_Scroll !=0.0f && m_CurrentlySelectedObject == id){
					m_FontSystem->ChangeArrowOffset(m_InputTextData[id].LineStarts[it->second.ScrollYIndex]);
				}
		}
		if(m_InputTextInputEvent){

		UpdateLineStarts(m_InputTextData[id].LineStarts,Buffer,BufferSize,0);
		//make the pointer functional again with added scrolling.
		m_InputTextInputEvent = false;
		}
		//calculate how much chars fit in this window 
		int64_t lineIndex = m_InputTextData[id].ScrollYIndex;

		if(lineIndex+1 >= m_InputTextData[id].LineStarts.size())
		lineIndex =m_InputTextData[id].LineStarts.size()-1;
		//logicalPosY= (lPosition.y+(Size.y*0.5f*0.1f/totalLogicalPosYSize));
		logicalPosY = lPosition.y+rSize.y-(lineIndex*rSize.y/totalLogicalPosYSize*2.0f)-rSize.y/totalLogicalPosYSize;

		m_FontSystem->InputText(ID, Buffer, BufferSize, lPosition, rSize,m_InputTextData[id].LineStarts[lineIndex]);

		renderer->DrawQuad({lPosition.x+rSize.x-rSize.x*0.1f,lPosition.y},sliderBarColor,{rSize.x*0.05f,rSize.y},0);

		renderer->DrawQuad({lPosition.x+rSize.x-rSize.x*0.1f,logicalPosY},sliderButtonColor,{rSize.x*0.05f,rSize.y/totalLogicalPosYSize},0);



	}else{
		m_FontSystem->InputText(ID, Buffer, BufferSize, lPosition, rSize);
	}


}
bool GUIRenderer::IsObjectHovered(const std::string& strID){
	return IsObjectHovered(Core::GetStringHash(strID));
}
bool GUIRenderer::IsObjectHovered(GUUID objID){
	GUUID id = Application::GetCurrentlyHoveredPixelID();
	if(id == objID)
		return true;
	return false;
}
void GUIRenderer::ApplyCurrentStyles( Float2& position, Float2& size, Float4& color,GUUID id){
	Render* renderer= Application::GetRender();
	for(uint32_t i=0 ; i < m_StylesInQueue.size();i++){
		if(m_StylesInQueue[i].empty())
			continue;
		StyleContainer& container = m_StylesInQueue[i].top();
		switch(container.StyleType){
			case GUI::Style::BORDER:{
				GUI::BorderStyle& border = *(GUI::BorderStyle*)container.StyleData;

				if(border.DrawBorder)
					renderer->DrawQuad({position.x,position.y},border.BorderColor,{size.x+border.BorderWidth,size.y+border.BorderWidth},id.ID);
				renderer->DrawQuad({position.x,position.y},border.BackGroundColor,{size.x,size.y},id.ID);
				break;
			}
			case GUI::Style::TOOLTIP:{
				
				TooltipBackEndData backEndData{};
				for(uint32_t j=0 ;j < m_StylesInQueue[(uint32_t)GUI::Style::TOOLTIP-1].size();){	
				TooltipData& tooltip = *(TooltipData*)m_StylesInQueue[(uint32_t)GUI::Style::TOOLTIP-1].top().StyleData;
				m_StylesInQueue[(uint32_t)GUI::Style::TOOLTIP-1].pop();
				
				backEndData.tooltip = tooltip.tooltip;
				backEndData.Pos = {position.x+tooltip.PosOffset.x,position.y+tooltip.PosOffset.y};
				backEndData.Size = tooltip.MaxSize;
				backEndData.BackGroundColor = tooltip.BackGroundColor;
				backEndData.BorderColor = tooltip.BorderColor;
				backEndData.ID = tooltip.ID;

				m_TooltipStack.push(backEndData);

			
				delete &tooltip;
				}
				break;
			}

			default:{
				Core::Log("This type is not valid or is not implemented type =",(uint32_t)container.StyleType);
				break;
			}
		}



	}
}
void GUIRenderer::ResetGUIData(){
	m_Buttons.clear();
	m_Panels.clear();
	m_Sliders.clear();
	m_CheckBoxes.clear();
	m_InputTextData.clear();
	m_PanelIDs.clear();
}
void GUIRenderer::SetFont(const std::string& strID,uint32_t fontSize){
	//m_FontSystem->SetCharcterSize(14);
	//Asset<Font> newFont = m_FontSystem->LoadFont(strID);
	std::string name =Core::GetFileName(strID);
	if(fontSize ==0){
		fontSize = Application::GetRenderer()->GetCurrentFont().GetData()->FontSize;
	}

	auto newFont = Application::GetAsset<Font>(FONTID(name,fontSize));
	Render* render = Application::GetRender();
	render->SetFont(newFont,fontSize);
}
void GUIRenderer::SetFont(GUUID id,uint32_t fontSize){
	//m_FontSystem->SetCharcterSize(14);
	Asset<Font> newFont = Application::GetAsset<Font>(id);
	Render* render = Application::GetRender();
	render->SetFont(newFont,fontSize);
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
	m_StylesInQueue[(uint32_t)style-1].push({style,Data});
	m_LastAddedStyle = style;


}
void GUIRenderer::PopStyle() {
	if(!m_StylesInQueue[(uint32_t)m_LastAddedStyle-1].empty())
		m_StylesInQueue[(uint32_t)m_LastAddedStyle-1].pop();
	
}
uint32_t GUIRenderer::GetFontSize() {
	return m_FontSystem->GetFontSize();
}
void GUIRenderer::OnEvent(Event& event){
	if(event.GetEventType() == EventType::KEYBOARD)
		OnKeyBoardEvent((KeyBoardEvent&)event);
	else if(event.GetEventType() == EventType::MOUSE)
		OnMouseEvent((MouseEvent&)event);
	else if(event.GetEventType() == EventType::INPUTTEXTEVENT)
		OnInputTextEvent((InputTextEvent&)event);
}
void GUIRenderer::OnInputTextEvent(InputTextEvent& event){
	if(m_CurrentlySelectedObject !=0){
		auto it = m_InputTextData.find(m_CurrentlySelectedObject);
		if(it != m_InputTextData.end()){
			m_InputTextInputEvent = true;
		}

	}


}
void GUIRenderer::OnKeyBoardEvent(KeyBoardEvent& event){

}
void GUIRenderer::OnMouseEvent(MouseEvent& event){
	Renderer* renderer = Application::GetRenderer();
	Float2 data{};
	bool succeded = Core::ReadPixel(m_PickBufferData,renderer->GetViewPortExtent().width,renderer->GetViewPortExtent().height,Application::GetMousePos().x,Application::GetMousePos().y,&data);

	if(event.Code == MouseCodes::LEFT && event.State == EventState::PRESSED){
		if(succeded){
			m_SelectedObjID = *(uint64_t*)&data;
			m_CurrentlySelectedObject = m_SelectedObjID;
			m_DraggedPanel = m_SelectedObjID;
		}


	}

	if(event.Code == MouseCodes::LEFT && event.State == EventState::RELEASED){
		m_SelectedObjID = 0;
		m_DraggedPanel =0;
	}
	if(event.Code == MouseCodes::SCROLL){
		if(event.ScrollY > 0.0f)
		m_Scroll = 1.0f;
		else 
		m_Scroll = -1.0f;
	}


}
void GUIRenderer::DrawTooltips(){
	Render* renderer = Application::GetRender();

	for(uint32_t i=0;i < m_TooltipStack.size();){
		TooltipBackEndData& data = m_TooltipStack.top();
		//Draw border
		if(data.BorderColor.a != 0.0f)
			renderer->DrawQuad({data.Pos.x,data.Pos.y},data.BorderColor,{data.Size.x+0.01f,data.Size.y+0.01f},data.ID.ID);
		//Draw background
		if(data.BackGroundColor.a !=0.0f)
			renderer->DrawQuad({data.Pos.x,data.Pos.y},data.BackGroundColor,data.Size,data.ID.ID);
		m_FontSystem->Text(data.ID,data.tooltip.c_str(),data.Pos,data.Size);
		m_TooltipStack.pop();

	}
	
}
void GUIRenderer::EndGUI()
{
	DrawTooltips();
	m_Scroll = 0.0f;
	m_SelectedObjID = 0;
	//Update the dragged panel/button
	m_CurrentPanel = 0;

	//delete button that hasn't been used.
}
	GUIRenderer::~GUIRenderer(){

		delete[] m_PickBufferData;
	}

