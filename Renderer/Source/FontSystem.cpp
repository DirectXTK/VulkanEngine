#include "FontSystem.h"
#include <Renderer.h>
#include "Application.h"
#include "CommandBuffer.h"
#include "freetype/ftglyph.h"
FontSystem* g_FontSystem{};
FontSystem::FontSystem()
{
	m_Renderer = Application::GetRenderer();
	g_FontSystem = this;
	FT_Error error = FT_Init_FreeType(&m_Library);
	if (error) 
		Core::Log(ErrorType::Error, "Failed to initialize FreeType.");

	//try to load any font or a default one
	const char* FontPath = "/users/jimy/Repos/VulkanEngine/Resources/Fonts/Sacrifice.ttf";
	//const char* FontPath = "/users/jimy/Repos/VulkanEngine/Resources/Fonts/Daydream.ttf";
	//Day dream causes crashes.
	m_CurrentFont = LoadFont(FontPath);
	m_Renderer->SetCurrentFont(m_CurrentFont);
}
Asset<Font> FontSystem::LoadFont(const std::string& filePath){
	//loads the font
	FT_Error error = FT_New_Face(m_Library, filePath.c_str(), 0, &m_Face);
	if (error == FT_Err_Unknown_File_Format) {
		Core::Log(ErrorType::Error, "Unknown file format of font");
	}
	else if (error) {
		Core::Log(ErrorType::Error, "Failed to open/read or the font is broken ");
		return Asset<Font>();
	}

	std::string Name = Core::GetFileName(filePath);
	Asset<Font> asset= ReRenderFaces("FONT"+Name+std::to_string(m_CharacterSize),Core::GetFileName(filePath));
	//LASTERROR
	//FT_Done_Face(m_Face);
	return asset;
}

void FontSystem::Run(void* app,void* iRenderer)
{
	
}

void FontSystem::SetCharcterSize(uint32_t CharSize)
{
	if (CharSize == m_CharacterSize)
		return;

	m_CharacterSize = CharSize;

	if(m_CurrentFont){
		m_CurrentFont = ReRenderFaces("FONT"+m_CurrentFont.GetData()->FontName+std::to_string(m_CharacterSize),m_CurrentFont.GetData()->FontName);
		m_Renderer->SetCurrentFont(m_CurrentFont);

	}
}

uint32_t FontSystem::GetWidthOfChar()
{
	return 0;
}

uint32_t FontSystem::GetHeightOfChar()
{
	return 0;
}

Texture* FontSystem::GetFontAtlas()
{
	return m_FontAtlas;
}
void FontSystem::SetFont(const std::string& fontPath){
	std::string absPath = std::filesystem::absolute(fontPath);
	Asset<Font> asset = LoadFont(absPath);
	if(asset){
		m_CurrentFont = asset;
		Application::GetRenderer()->SetCurrentFont(m_CurrentFont);
		return;
	}
	Core::Log("Failed to FontSystem::SetFont(){path=",absPath,"}");
}
void FontSystem::SetFont(GUUID ID)
{
	Asset<Font> font = Application::GetAsset<Font>(ID);
	if(font){
		m_CurrentFont = font;
		Application::GetRenderer()->SetCurrentFont(m_CurrentFont);
		return;
	}
	Core::Log(ErrorType::Error,"Failed to PushFont ID{",ID.ID,"]");
	return;
}
void FontSystem::OnEvent(Event& event){
	if(event.GetEventType() == EventType::KEYBOARD)
		OnKeyBoardEvent((KeyBoardEvent&)event);
	if(event.GetEventType() == EventType::MOUSE)
		OnMouseEvent((MouseEvent&)event);
	if(event.GetEventType() == EventType::TEXTEVENT)
		OnTextEvent((TextEvent&)event);
}
uint64_t FontSystem::FindMousePosInText(const Float2& mousePos,char* Buffer,uint64_t BufferSize,const Float2& Position,const Float2& size){
	Float2 arrowPos{Core::ToScreenPixels(Position)};
	float arrowPixelY = Core::ToScreenPixels(mousePos).y;
	float smallestDist{std::numeric_limits<float>::max()};
	uint32_t currentLine{0};
	Float2 sizeInPixels = Core::ToScreenPixels(	{size.x,size.y});
	

	arrowPixelY = std::fabs(arrowPixelY-arrowPos.y);
	arrowPixelY = size.y*Application::GetRenderer()->GetViewPortExtent().height/m_CurrentFont.GetData()->NewLineSize;
	
	arrowPixelY -=(((arrowPos.y-Core::ToScreenPixels(mousePos).y))/m_CurrentFont.GetData()->NewLineSize); 

	arrowPixelY = std::floor(arrowPixelY);
	for(uint32_t i =0;i < BufferSize;i++){

		if(Buffer[i] == '\0'){
			return i;
		}
		if(Buffer[i] == '\n'||arrowPos.x+m_CurrentFont.GetData()->Advance[Buffer[i]].x >= sizeInPixels.x){
			if(currentLine == arrowPixelY){
				return i;
			}
			arrowPos = Core::ToScreenPixels(Position);
			currentLine++;
		}
		arrowPos.x += m_CurrentFont.GetData()->Advance[Buffer[i]].x*0.5f;
		
		if(currentLine == arrowPixelY){
			
		

		//arrowPos.y +=m_CurrentFont.GetData()->Advance[Buffer[i]].y;

		Float2 normArrowPos = Core::ToNDC(arrowPos);
		float temp = std::fabs(normArrowPos.x-mousePos.x);
		if(mousePos.x < normArrowPos.x){
			return i;
		}
		}
		arrowPos.x += m_CurrentFont.GetData()->Advance[Buffer[i]].x*0.5f;


		
	}
	return 0;
}
void FontSystem::OnMouseEvent(MouseEvent& event){
	if(event.State == EventState::PRESSED && event.Code == MouseCodes::LEFT){

		GUUID selectedID = Application::GetCurrentlyHoveredPixelID();

		auto it = m_InputTextData.find(selectedID);

		if(it != m_InputTextData.end()){
			if(m_CurrentlySelectedInputData != selectedID){
				m_ArrowPosition =0;
				m_ArrowPositionOffset =0;
			}
			m_CurrentlySelectedInputData = selectedID;
			m_IsArrowActive = true;
			//indicate to find the position.
			m_ArrowPosition=std::numeric_limits<uint64_t>::max();

		}else{
			m_CurrentlySelectedInputData = 0;
			m_IsArrowActive = false;
			m_ArrowPosition=0;
		}
	}
}
void FontSystem::OnTextEvent(TextEvent& event){
	char insertedChar= event.KeyChar;
			
			
		if(m_CurrentlySelectedInputData !=0){
			auto it = m_InputTextData.find(m_CurrentlySelectedInputData);
			if(it != m_InputTextData.end()){
			InputTextData data = it->second;

			memccpy(data.buffer+m_ArrowPosition+1,data.buffer+m_ArrowPosition,0,data.bufferSize-m_ArrowPosition);
			data.buffer[m_ArrowPosition] = insertedChar;
			if(m_ArrowPosition != data.bufferSize-1)
				m_ArrowPosition++;
			InputTextEvent event{};
			event.AddedChar = insertedChar;
			Application::DispatchEvent(event);
			}
		}
}
void FontSystem::OnKeyBoardEvent(KeyBoardEvent& event){
	if(event.State == EventState::PRESSED||event.State == EventState::HOLD&& m_CurrentlySelectedInputData != 0){
		auto it =m_InputTextData.find(m_CurrentlySelectedInputData);
		if(it != m_InputTextData.end()){
		InputTextData data = it->second;
		if(event.Key == KeyCodes::ARROWLEFT){
			if(m_ArrowPosition != 0)
				m_ArrowPosition--;
		}
		else if(event.Key == KeyCodes::ARROWRIGHT){
			if(m_ArrowPosition != data.bufferSize-1)
				m_ArrowPosition++;
		}
		else if(event.Key == KeyCodes::ARROWUP){

		}
		else if(event.Key == KeyCodes::ARROWDOWN){
			
		}else if(event.Key == KeyCodes::BACKSPACE){
			if(m_ArrowPosition != 0)
			{
				InputTextEvent event{};
				event.RemovedChar = data.buffer[m_ArrowPosition-1];
				memccpy(data.buffer+m_ArrowPosition-1,data.buffer+m_ArrowPosition,0,data.bufferSize-m_ArrowPosition);
				m_ArrowPosition--;

				Application::DispatchEvent(event);
			}
		}else if(event.Key == KeyCodes::ENTER){
			char insertedChar = '\n';

			memccpy(data.buffer+m_ArrowPosition+1,data.buffer+m_ArrowPosition,0,data.bufferSize-m_ArrowPosition);
			data.buffer[m_ArrowPosition] = insertedChar;
			if(m_ArrowPosition != data.bufferSize-1)
				m_ArrowPosition++;
			InputTextEvent event{};
			event.AddedChar = insertedChar;
			Application::DispatchEvent(event);
		}
		
	}
	}
	if(event.Key == KeyCodes::CTRL){
		if(event.State == EventState::PRESSED)
			m_ControlKey = true;
		if(event.State == EventState::RELEASED)
			m_ControlKey = false;

	}

	if(m_ControlKey&&event.Key == KeyCodes::C && event.State == EventState::PRESSED){
		m_CopyTriggered = true;
	}else{
		m_CopyTriggered =false;
	}




}
void FontSystem::ChangeArrowOffset(int32_t offset){
	m_ArrowPositionOffset = offset;

}
void FontSystem::InputText(const char* ID, char* Buffer,uint64_t BufferSize, Float2 Position, Float2 Size,uint64_t stringOffset)
{
	GUUID SelectID = Core::GetStringHash(ID);
	bool ScrollableBoundBox{};


	Float2 BoundingBox[4];
	BoundingBox[0] = { Position.x - Size.x,Position.y- Size.y  };
	BoundingBox[1] = { Position.x - Size.x,Position.y + Size.y };
	BoundingBox[2] = { Position.x + Size.x,Position.y + Size.y };
	BoundingBox[3] = { Position.x + Size.x,Position.y - Size.y};
	
	if(m_CopyTriggered&&m_CurrentlySelectedInputData == SelectID){
		Application::CopyToClipBoard(std::string_view(Buffer,BufferSize));
		m_CopyTriggered = false;
	}

	m_InputTextData[SelectID] = {BufferSize,Buffer,Size};
	BufferSize-stringOffset;
	Buffer+=stringOffset;
	//Draw the invisible barrier that  provides the selecting 

	if(m_CurrentlySelectedInputData == SelectID){
		m_PointerCooldown -= Application::GetDeltaTime();
		if(!m_IsArrowActive){
			if(m_PointerCooldown <=0.0f)
				m_IsArrowActive = true;
		}
		//fins the pos according to mouse pos
		if(m_ArrowPosition == std::numeric_limits<uint64_t>::max()){

			m_ArrowPosition = FindMousePosInText(Application::GetMousePosNorm(),Buffer,BufferSize,{BoundingBox[0].x,BoundingBox[0].y},Size)+m_ArrowPositionOffset;
		}


		if (m_IsArrowActive ) {
			if(m_PointerCooldown <= -m_PointerBlinkCooldownConst){			
				m_PointerCooldown = m_PointerBlinkCooldownConst;
				m_IsArrowActive = false;
				
			}
			m_Renderer->RenderText(Buffer,BufferSize,{BoundingBox[0].x,BoundingBox[1].y}, BoundingBox, m_Padding, m_CharacterSize, SelectID, m_ArrowPosition-m_ArrowPositionOffset);
			return;
		}
	}
	m_Renderer->RenderText(Buffer, BufferSize,{BoundingBox[0].x,BoundingBox[1].y}, BoundingBox, m_Padding, m_CharacterSize, SelectID);
}
void FontSystem::Text(const char* StrId,const char* Message, Float2 Position,Float2 MaxSize)
{

	Renderer* renderer = Application::GetRenderer();

	GUUID SelectID = Core::GetStringHash(StrId);
	Float2 CharacterSizeNorm = { float(m_CharacterSize   / renderer->GetViewPortExtent().width*0.5f),float(m_CharacterSize/ renderer->GetViewPortExtent().height*0.5f) };

	Float2 Size{};

	

	if (MaxSize.x != 0) {
		Size.x = MaxSize.x;
	}
	else {
		Size.x = ((m_Padding + m_Padding  + CharacterSizeNorm.x) * strlen(Message)) + m_Padding;

	}

	if (MaxSize.y!= 0) {
		Size.y = MaxSize.y;

	}
	else if(MaxSize.x !=0){
		Size.y = strlen(Message)/(m_PaddingY + m_PaddingY + CharacterSizeNorm.x);
	}
	else {
		Size.y = m_PaddingY+ m_PaddingY + CharacterSizeNorm.y;
	}
	

	Float2 BoundingBox[4];
	BoundingBox[0] = { Position.x ,Position.y };
	BoundingBox[1] = { Position.x ,Position.y + Size.y };
	BoundingBox[2] = { Position.x + Size.x,Position.y + Size.y };
	BoundingBox[3] = { Position.x + Size.x,Position.y };

	DrawBorder(Position, Size, SelectID);


	renderer->RenderText(Message,strlen(Message), { BoundingBox[0].x,BoundingBox[1].y }, BoundingBox, m_Padding, m_CharacterSize, SelectID);

}
void FontSystem:: Text(GUUID id, const char* Message, Float2 Position, Float2 MaxSize)
{
	Renderer* renderer = Application::GetRenderer();
	
	GUUID SelectID =id;
	Float2 CharacterSizeNorm = { m_CurrentFont.GetData()->Advance[(uint32_t)' '].x/Application::GetRenderer()->GetViewPortExtent().width,m_CharacterSize*1.25f/Application::GetRenderer()->GetViewPortExtent().height};

	Float2 Size{};



	if (MaxSize.x != 0.0f) {
		Size.x = MaxSize.x;
	}
	else {
		Size.x = CharacterSizeNorm.x * (strlen(Message)+2)*4;
		Core::Log("Sizex",Size.x,",",CharacterSizeNorm.x);
	}

	if (MaxSize.y != 0.0f) {
		Size.y = MaxSize.y;

	}
	else {
		Size.y = CharacterSizeNorm.y * strlen(Message);
		Core::Log("SizeY",Size.y,",",CharacterSizeNorm.y);
	}

	Float2 BoundingBox[4];
	BoundingBox[0] = { Position.x-Size.x ,Position.y-Size.y };
	BoundingBox[1] = { Position.x -Size.x,Position.y + Size.y };
	BoundingBox[2] = { Position.x + Size.x,Position.y + Size.y };
	BoundingBox[3] = { Position.x + Size.x,Position.y-Size.y };

	//DrawBorder(Position, Size, SelectID);


	renderer->RenderText(Message,strlen(Message), { BoundingBox[0].x,BoundingBox[1].y }, BoundingBox, m_Padding, m_CharacterSize, SelectID);

}
void FontSystem::DrawBorder(Float2& Position,Float2& Size,GUUID ID)
{
	Renderer* renderer = Application::GetRenderer();

	
		renderer->DrawQuad({ Position.x ,Position.y ,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { Size.x ,Size.y  }, ID.ID);
}


void FontSystem::KeyBoardCallback(KeyBoardEvent* event)
{
	m_KeyStates.push(event->State);
	m_KeyCodes.push(event->Key);
	if(event->State == EventState::RELEASED)
		m_KeyAlreadyPressed[(uint32_t)event->Key] = false;

}

FontSystem::~FontSystem()
{
	FT_Done_FreeType(m_Library);
}
void FontSystem::SpecialCases(KeyCodes& Code, EventState& State, char* Buffer, uint64_t Size)
{
	//is num lock is off
	if ((int)Code >= 320 && (int)Code <= 329)
		(int&)Code -= 272;

	switch (Code) {
	case KeyCodes::BACKSPACE: {
		if (m_CharEditedIndex > 0) {
			memcpy(Buffer + m_CharEditedIndex - 1, Buffer + m_CharEditedIndex, Size - m_CharEditedIndex);
			m_CharEditedIndex--;
		}
			
		break;
	}
	case KeyCodes::ENTER: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '\n';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM0: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '0';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM1: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '1';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM2: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '2';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM3: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '3';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM4: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '4';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM5: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '5';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM6: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '6';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM7: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '7';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM8: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '8';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::NUM9: {
		memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
		Buffer[m_CharEditedIndex] = '9';

		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::ARROWRIGHT: {
		m_CharEditedIndex++;
		break;
	}
	case KeyCodes::ARROWLEFT: {
		if(m_CharEditedIndex!=0)
			m_CharEditedIndex--;
		break;

	}


	default: {
		Core::Log(ErrorType::Warning, "Not implemented yet");
		break;
	}
	}
}
void FontSystem::DrawPointer(Float2 Position, float CharacterSize,float SizeY)
{
	Renderer* renderer = Application::GetRenderer();
	//renderer->DrawQuad({ Position.x,Position.y }, { 1.0f,1.0f,1.0f,1.0f }, { m_Padding * CharacterSize ,SizeY},0);
}
Asset<Font> FontSystem::ReRenderFaces(GUUID fontID,const std::string& fontName)
{
	FT_GlyphSlot slot = m_Face->glyph;
	AssetManager* manager = Application::GetAssetManager();
	float dpi = Application::GetRenderer()->GetFONTDPI();
	//check if font is already loaded and renderer
	Asset<Font> asset = manager->GetAsset<Font>(fontID);
	if(asset){
		if(asset.GetData()->FontSize == m_CharacterSize)
			return asset;
	}


	FT_Error error = FT_Set_Char_Size(m_Face, 0, (m_CharacterSize)*64, (uint32_t)dpi, (uint32_t)dpi);
	if (error) {
		Core::Log(ErrorType::Error, "Failed to set the font char size");
	}

	float FontAtlasWidth{}, FontAtlasHeight{};
	uint32_t SubTextureIndex{};
	float OffsetX{0};
	float OffsetY{0};
	TextureCoords* AtlasCoords{};
	Float2* MinCord{}, *MaxCord{};
	Float2* advance{};
	float SizeX{16};
	float SizeY{ 16 };
	int64_t MaxY{};

	FontAtlasWidth = 2048;
	FontAtlasHeight = 2048;
	//if asset isint loaded create new one else just update the values
	if(!asset){
	AtlasCoords = new TextureCoords[m_Face->num_glyphs];
	MinCord = new Float2[m_Face->num_glyphs];
	MaxCord = new Float2[m_Face->num_glyphs];
	advance = new Float2[m_Face->num_glyphs];

	memset(AtlasCoords,0x00000000, m_Face->num_glyphs*sizeof(TextureCoords));
	for(uint32_t i =0 ;i < m_Face->num_glyphs;i++){
		MinCord[i] = {0.0f,0.0f};
		MaxCord[i] = {0.0f,0.0f};
		advance[i] = {0.0f,0.0f};
	}
	}else{
		Font* font = asset.GetData();
		AtlasCoords = font->Coords;

		MinCord = font->MinCord;
		MaxCord = font->MaxCord;
		advance = font->Advance;
		font->FontSize = m_CharacterSize;


	}

	error = FT_Select_Charmap(m_Face, FT_ENCODING_UNICODE);
	if (error)
		Core::Log(ErrorType::Error, "Failed to select charmap.");

	uint32_t* AtlasMapBitmap = new uint32_t[(uint32_t)FontAtlasWidth * (uint32_t)FontAtlasHeight];
	memset(AtlasMapBitmap,0x00000000, FontAtlasWidth * FontAtlasHeight*sizeof(uint32_t));

	for (uint32_t i = 0x20; i < std::min((uint32_t)0x7E,(uint32_t)m_Face->num_glyphs); i++) {
		uint32_t GlyphIndex = FT_Get_Char_Index(m_Face, i);
		SubTextureIndex = i;

		if (GlyphIndex == 0){
			MinCord[SubTextureIndex] = {0.0f,0.0f};
			MaxCord[SubTextureIndex] = {0.0f,0.0f};
			continue;
		}
		
		FT_Error error = FT_Load_Glyph(m_Face, GlyphIndex, FT_LOAD_DEFAULT);
		if (error) {
			Core::Log(ErrorType::Error, "Failed to load glyph");
			continue;
		}

		slot = m_Face->glyph;

		error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
		if (error) {
			Core::Log(ErrorType::Error, "Failed to render glyph");
			continue;
		}
		
		advance[SubTextureIndex].x = (slot->advance.x>>6);
		advance[SubTextureIndex].y = (slot->advance.y>>6);
		if (slot->bitmap.rows == 0 || slot->bitmap.width == 0){
			MinCord[SubTextureIndex] = {0.0f,0.0f};
			MaxCord[SubTextureIndex] = {0.0f,0.0f};
			continue;
		}
		
		
		AtlasCoords[SubTextureIndex].Width = slot->bitmap.width;
		AtlasCoords[SubTextureIndex].Height = slot->bitmap.rows;
		SizeX = (float)slot->bitmap.width;
		SizeY = (float)slot->bitmap.rows;
			

		if (OffsetX + AtlasCoords[SubTextureIndex].Width > FontAtlasWidth) {
			OffsetY += MaxY;
			MaxY = 0;
			OffsetX = 0;
		}
		MaxY = std::max(MaxY, (int64_t)slot->bitmap.rows);

		//add offest but reduce the size if the size is lower add padding to seem
		AtlasCoords[SubTextureIndex].Coords[0] = { float(OffsetX / (float)FontAtlasWidth),float((SizeY+OffsetY) / (float)FontAtlasHeight) };
		AtlasCoords[SubTextureIndex].Coords[1] = { float(OffsetX / (float)FontAtlasWidth),float(OffsetY / (float)FontAtlasHeight) };
		AtlasCoords[SubTextureIndex].Coords[2] = { float((SizeX+OffsetX) / (float)FontAtlasWidth),float(OffsetY / (float)FontAtlasHeight) };
		AtlasCoords[SubTextureIndex].Coords[3] = { float((SizeX+OffsetX) / (float)FontAtlasWidth) ,float((SizeY+OffsetY) / (float)FontAtlasHeight)  };

		FT_Glyph glyph{};
		FT_BBox box{};

		error = FT_Get_Glyph(slot, &glyph);
		if (error) {
			Core::Log(ErrorType::Error, "Failed to get glyph from slot.");
		}

		float glyphLeft  =  slot->bitmap_left;
		float glyphRight = glyphLeft + slot->bitmap.width;

		// 3. Vertical positions using metrics
		float glyphTop    = (slot->metrics.horiBearingY >> 6); // top relative to baseline
		float glyphBottom = glyphTop - (slot->metrics.height >> 6);  

			MinCord[SubTextureIndex].x = glyphLeft;
		MinCord[SubTextureIndex].y = glyphTop;
			
		MaxCord[SubTextureIndex].x = (float)glyphRight ;
		MaxCord[SubTextureIndex].y =  glyphBottom;

		advance[SubTextureIndex].x = (slot->advance.x>>6);
		advance[SubTextureIndex].y = (slot->advance.y>>6);


		for (uint32_t x = 0; x < slot->bitmap.width; x++) {	
			for (uint32_t y = 0; y < slot->bitmap.rows; y++) {
				AtlasMapBitmap[(uint32_t)OffsetX+x + ((y + (uint32_t)OffsetY)* (uint32_t)FontAtlasWidth )] |= uint32_t(slot->bitmap.buffer[(y * slot->bitmap.width+x)]<<24);
			}
		}
		OffsetX += (float)AtlasCoords[SubTextureIndex].Width;
		FT_Done_Glyph(glyph);
	}
	
	if(!asset){
	Font* font = new Font();
	font->Coords = AtlasCoords;
	font->MinCord = MinCord;
	font->MaxCord = MaxCord;
	font->FontSize = m_CharacterSize;
	font->GlyphCount = m_Face->num_glyphs;
	font->TextureID = Core::GetStringHash("FontTexture:"+std::to_string(m_CharacterSize)+":"+fontName);
	font->Advance = advance;
	font->FontName = fontName;
	font->NewLineSize = 1.25f*m_CharacterSize;

	m_TextureSize = { FontAtlasWidth,FontAtlasHeight };

	TextureCreateInfo textureCreateInfo{};
	textureCreateInfo.ImageUsageFlags = VkImageUsageFlagBits(VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT);
	textureCreateInfo.Format = VK_FORMAT_R8G8B8A8_UNORM;
	textureCreateInfo.Width = FontAtlasWidth;
	textureCreateInfo.Height = FontAtlasHeight;
	textureCreateInfo.Pixels = AtlasMapBitmap;
	textureCreateInfo.ImageLayout =VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	Texture* texture = new Texture(Application::GetRenderer()->GetContext(),textureCreateInfo,TextureType::Texture);
	font->TextureAsset = manager->LoadAsset<Texture>(texture, AssetType::TEXTURE, "FontTexture:"+std::to_string(m_CharacterSize)+":"+fontName);
	asset = manager->LoadAssetPerma<Font>(font, AssetType::FONT, fontID);
	}else{
		Texture* texture = asset.GetData()->TextureAsset.GetData();
		Context context = m_Renderer->GetContext();

		BufferDesc desc{};
		desc.Device = context->Device;
		desc.Memoryflags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		desc.Physdevice = context->PDevice;
		desc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
		desc.SizeBytes = 4*FontAtlasWidth*FontAtlasHeight;
		desc.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT|VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

		Buffer* buffer = new Buffer(desc);
		buffer->UploadToBuffer(context->Device,AtlasMapBitmap,0);
		context->m_TempBuffers->push_back(buffer);
		texture->TrasitionFormat(true,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,context->TransferCommandBuffer);
		texture->CopyFromBuffer(context->Device,buffer,context->TransferCommandBuffer);
		texture->TrasitionFormat(false,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,context->TransferCommandBuffer);
		
	}
	delete[] AtlasMapBitmap;
	return asset;
}
void FontSystem::PushStyle(const GUI::Style& style,void* StyleData) {
	m_Style.push(style);
	switch (style) {
	case GUI::Style::BORDER :{
		
		//Store style data somehow and transfer it.
			m_StyleData.push(malloc(sizeof(GUI::BorderStyle)));
			if (StyleData) {
				memcpy(m_StyleData.top(), StyleData, sizeof(GUI::BorderStyle));
			}

		break;

	}
	default: {
		Core::Log(ErrorType::Error, "This style doesn't exist");
		break;
	}
	}
}
void FontSystem::PopStyle() {
	free(m_StyleData.top());
	m_StyleData.pop();
	m_Style.pop();
}


