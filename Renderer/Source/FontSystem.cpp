#include "FontSystem.h"
#include "Renderer.h"
#include "Application.h"
#include "freetype/ftglyph.h"
FontSystem* g_FontSystem{};
void KeyBoardCallbackFn(KeyBoardEvent* event) {
	g_FontSystem->KeyBoardCallback(event);
}
FontSystem::FontSystem(Application* App)
{
	m_App = App;
	m_Renderer = App->m_Renderer;
	g_FontSystem = this;

	const char* FontPath = "C:\\Repos\\VulkanEngine\\Resources\\Fonts\\Daydream.ttf";
	Application* CurrentApp = (Application*)App;

	FT_Error error = FT_Init_FreeType(&m_Library);
	if (error) {
		Core::Log(ErrorType::Error, "Failed to initialize FreeType.");
	}
	//loads the font
	error = FT_New_Face(m_Library, FontPath, 0, &m_Face);
	if (error == FT_Err_Unknown_File_Format) {
		Core::Log(ErrorType::Error, "Unknown file format of font");
	}
	else if (error) {
		Core::Log(ErrorType::Error, "Failed to open/read or the font is broken ");
	}
	ReRenderFaces();
	InputCallbacks callbacks{};
	callbacks.KeyBoardCallback = KeyBoardCallbackFn;

	CurrentApp->AddCallback(&callbacks);

}

void FontSystem::Run(void* app,void* iRenderer)
{
	
}

void FontSystem::SetCharcterSize(float CharSize)
{
	if (CharSize == m_CharacterSize)
		return;

	m_CharacterSize = CharSize;
	m_Padding =( m_CharacterSize * 0.1f)/m_Renderer->GetViewPortExtent().width;
	m_PaddingY = (m_CharacterSize * 0.25f) / m_Renderer->GetViewPortExtent().height;

	ReRenderFaces();
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

void FontSystem::PushFont()
{
}

void FontSystem::InputText(const char* ID, char* Buffer,uint64_t BufferSize, Float2 Position, Float2 Size)
{
	GUUID SelectID = Core::GetStringHash(ID);
	bool ScrollableBoundBox{};

	m_PointerCooldown -= m_App->GetDeltaTime();

	Float2 BoundingBox[4];
	BoundingBox[0] = { Position.x ,Position.y  };
	BoundingBox[1] = { Position.x ,Position.y + Size.y };
	BoundingBox[2] = { Position.x + Size.x,Position.y + Size.y };
	BoundingBox[3] = { Position.x + Size.x,Position.y };
	


	if (m_App->m_InputSystem.IsMouseClicked(MouseCodes::LEFT)) {
		if (m_App->GetCurrentlyHoveredPixelID() == Core::GetStringHash(ID)){
			Float2 MousePos = m_App->GetMousePosNorm();
			float PosXInBox = std::fabs(BoundingBox[0].x - MousePos.x);
			m_CharEditedIndex = PosXInBox / ((m_CharacterSize+m_Padding+ m_Padding)/m_Renderer->GetViewPortExtent().width);


			//if its one of the special symbols make it not editable and if its the first char make it editable
 			if (Buffer[m_CharEditedIndex] <= 32) {

				if (m_CharEditedIndex == 0) {
					//do nothing
				}
				else if (Buffer[m_CharEditedIndex-1] <= 32)
				{
					m_CharEditedIndex = strlen(Buffer);
				}
				
				
			}
			else if (strlen(Buffer) == 0) {
				m_CharEditedIndex = 0;
			}
		}
		else {
			m_CharEditedIndex = -1;
		}
	}
	
	//Draw the invisible barrier that  provides the selecting 
	DrawBorder(Position, Size, SelectID);


	if (m_PointerCooldown <= 0.0f) {
		if (m_PointerCooldown <= -m_PointerBlinkCooldownConst)
			m_PointerCooldown = m_PointerBlinkCooldownConst;
		m_Renderer->RenderText(Buffer, { BoundingBox[0].x,BoundingBox[1].y - m_CharacterSize }, BoundingBox, m_Padding, m_CharacterSize, SelectID, m_CharEditedIndex);
	}
	else {
		m_Renderer->RenderText(Buffer, { BoundingBox[0].x,BoundingBox[1].y - m_CharacterSize }, BoundingBox, m_Padding, m_CharacterSize, SelectID);
	}
		//the spaces beetween letters are uneaven and the pointers sometimes isn't drawn.
		//draws in the center
		//renderer->DrawQuad({ BoundingBox[0].x + (m_CharEditedIndex * (CharacterSizeNorm + m_FixedPadding)) + (m_FixedPadding * 0.5f),BoundingBox[1].y - (CharacterSizeNorm * 1.0f),0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { m_FixedPadding * 0.5f ,CharacterSizeNorm }, 0);
	
	if (m_CharEditedIndex != -1) {
		for(uint32_t i=0;i < m_KeyStates.size();i++){
			KeyState State = m_KeyStates.back();
			KeyCodes Key = m_KeyCodes.back();
			if (State==KeyState::PRESSED || State == KeyState::HOLD) {

				if ((int)Key >= 32 && (int)Key <= 127) {
					if (strlen(Buffer) + 1 < BufferSize) {
						memcpy(Buffer + m_CharEditedIndex + 1, Buffer + m_CharEditedIndex, m_CharEditedIndex + 1);
						Buffer[m_CharEditedIndex] = (char)Key;

						m_CharEditedIndex++;
						m_TypingCooldown = m_CharEditCooldownConst;
					}
				}
				else {
					SpecialCases(Key, State, Buffer, BufferSize);
				}
				m_KeyAlreadyPressed[(uint32_t)Key] = true;

			
			}
			m_KeyCodes.pop();
			m_KeyStates.pop();
		}
			
		//pointer
		
		
	}



	

	//if (m_CharEditedIndex == -1)
		//Core::Log(ErrorType::Error, "Failed to find the char index.");
	//DrawPointer()
	// 
	
}
void FontSystem::Text(const char* StrId,const char* Message, Float2 Position,Float2 MaxSize)
{

	Application* app = (Application*)m_App;
	Renderer* renderer = ((Application*)m_App)->m_Renderer;

	GUUID SelectID = Core::GetStringHash(StrId);
	Float2 CharacterSizeNorm = { m_CharacterSize   / renderer->GetViewPortExtent().width,m_CharacterSize/ renderer->GetViewPortExtent().height };

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


	renderer->RenderText(Message, { BoundingBox[0].x,BoundingBox[1].y }, BoundingBox, m_Padding, m_CharacterSize, SelectID);

}
void FontSystem::Text(GUUID id, const char* Message, Float2 Position, Float2 MaxSize)
{

	Application* app = (Application*)m_App;
	Renderer* renderer = ((Application*)m_App)->m_Renderer;

	GUUID SelectID =id;
	Float2 CharacterSizeNorm = { m_CharacterSize / renderer->GetViewPortExtent().width,m_CharacterSize / renderer->GetViewPortExtent().height };

	Float2 Size{};



	if (MaxSize.x != 0) {
		Size.x = MaxSize.x;
	}
	else {
		Size.x = ((m_Padding + m_Padding + CharacterSizeNorm.x) * strlen(Message)) + m_Padding;

	}

	if (MaxSize.y != 0) {
		Size.y = MaxSize.y;

	}
	else if (MaxSize.x != 0) {
		Size.y = strlen(Message) / (m_PaddingY + m_PaddingY + CharacterSizeNorm.x);
	}
	else {
		Size.y = m_PaddingY + m_PaddingY + CharacterSizeNorm.y;
	}


	Float2 BoundingBox[4];
	BoundingBox[0] = { Position.x ,Position.y };
	BoundingBox[1] = { Position.x ,Position.y + Size.y };
	BoundingBox[2] = { Position.x + Size.x,Position.y + Size.y };
	BoundingBox[3] = { Position.x + Size.x,Position.y };

	DrawBorder(Position, Size, SelectID);


	renderer->RenderText(Message, { BoundingBox[0].x,BoundingBox[1].y }, BoundingBox, m_Padding, m_CharacterSize, SelectID);

}
void FontSystem::DrawBorder(Float2& Position,Float2& Size,GUUID ID)
{
	Application* app =(Application*)m_App;
	Renderer* renderer = (Renderer*)app->m_Renderer;

	
	Float4 DefBackGroundColor{ 0.2f,0.2f,0.2f,1.0f };

	if (m_Style.empty()) {
		renderer->DrawQuad({ Position.x + (Size.x * 0.5f),Position.y + (Size.y * 0.5f),0.0f }, { 1.0f,1.0f,1.0f,0.0f }, { Size.x * 0.5f,Size.y * 0.5f }, ID.ID);
		return;
	}

	//style switcthes
	switch (m_Style.top()) {
	case Style::DrawBorder: {

		StyleBorderData* BorderData = (StyleBorderData*)m_StyleData.top();

		renderer->DrawQuad({ Position.x + (Size.x * 0.5f),Position.y + (Size.y * 0.5f),0.0f }, BorderData->BorderColor, { (Size.x * 0.5f) + BorderData->BorderWidth,(Size.y * 0.5f)+ BorderData->BorderWidth }, ID.ID);
		renderer->DrawQuad({ Position.x + (Size.x * 0.5f),Position.y + (Size.y * 0.5f),0.0f }, BorderData->BackGroundColor, { Size.x * 0.5f,Size.y * 0.5f }, ID.ID);
		break;
	}
	default: {
		renderer->DrawQuad({ Position.x + (Size.x * 0.5f),Position.y + (Size.y * 0.5f),0.0f }, DefBackGroundColor, { Size.x * 0.5f,Size.y * 0.5f }, ID.ID);
		break;
	}
	}

}

void FontSystem::PopFont()
{
}

void FontSystem::KeyBoardCallback(KeyBoardEvent* event)
{
	m_KeyStates.push(event->State);
	m_KeyCodes.push(event->Key);
	if(event->State == KeyState::RELEASED)
		m_KeyAlreadyPressed[(uint32_t)event->Key] = false;

}

FontSystem::~FontSystem()
{
}
void FontSystem::SpecialCases(KeyCodes& Code, KeyState& State, char* Buffer, uint64_t Size)
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
	Application* app = (Application*)m_App;
	Renderer* renderer = app->m_Renderer;

	//renderer->DrawQuad({ Position.x,Position.y }, { 1.0f,1.0f,1.0f,1.0f }, { m_Padding * CharacterSize ,SizeY},0);
}
void FontSystem::ReRenderFaces()
{
	
	Application* app = (Application*)m_App;
	FT_Error error = FT_Set_Char_Size(m_Face, 0, m_CharacterSize*64, 96, 96);
	if (error) {
		Core::Log(ErrorType::Error, "Failed to set the font char size");
	}

	FT_GlyphSlot slot = m_Face->glyph;
	float FontAtlasWidth, FontAtlasHeight{};
	uint32_t SubTextureIndex{};
	uint32_t ChannelCount{};
	float OffsetX{0};
	float OffsetY{0};
	TextureCoords* AtlasCoords{};
	Float2* MinCord{}, *MaxCord{};
	Float2* SubTextureSizes{};
	float SizeX{16};
	float SizeY{ 16 };
	int64_t MaxY{};

	FontAtlasWidth = m_Face->max_advance_width;
	FontAtlasHeight = m_Face->max_advance_height;

	AtlasCoords = new TextureCoords[m_Face->num_glyphs];
	MinCord = new Float2[m_Face->num_glyphs];
	MaxCord = new Float2[m_Face->num_glyphs];

	SubTextureSizes = new Float2[m_Face->num_glyphs];

	error = FT_Select_Charmap(m_Face, FT_ENCODING_UNICODE);
	if (error)
		Core::Log(ErrorType::Error, "Failed to select charmap.");

	uint32_t* AtlasMapBitmap = new uint32_t[FontAtlasWidth * FontAtlasHeight];
	memset(AtlasMapBitmap,0x00000000, FontAtlasWidth * FontAtlasHeight*sizeof(uint32_t));


	for (uint32_t i = 0; i <= m_Face->num_glyphs; i++) {
		uint32_t GlyphIndex = FT_Get_Char_Index(m_Face, i);

		if (GlyphIndex == 0)
			continue;
		FT_Error error = FT_Load_Glyph(m_Face, GlyphIndex, FT_LOAD_DEFAULT);
		if (error) {
			Core::Log(ErrorType::Error, "Failed to load glyph");
			continue;
		}

		if (error)
			Core::Log(ErrorType::Error, "Failed to set pixel sizes.");

		error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
		if (error) {
			Core::Log(ErrorType::Error, "Failed to render glyph");
			continue;
		}

		if (slot->bitmap.rows == 0 || slot->bitmap.width == 0)
			continue;
		
		

		AtlasCoords[SubTextureIndex].Width = slot->bitmap.width;
		AtlasCoords[SubTextureIndex].Height = slot->bitmap.rows;
		SizeX = slot->bitmap.width;
		SizeY = slot->bitmap.rows;
		

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

		//round coords for no artifacts
		AtlasCoords[SubTextureIndex].Coords[0].x = std::roundf(AtlasCoords[SubTextureIndex].Coords[0].x * 1000.f) / 1000.f;
		AtlasCoords[SubTextureIndex].Coords[1].x = std::roundf(AtlasCoords[SubTextureIndex].Coords[1].x * 1000.f) / 1000.f;
		AtlasCoords[SubTextureIndex].Coords[2].x = std::roundf(AtlasCoords[SubTextureIndex].Coords[2].x * 1000.f) / 1000.f;
		AtlasCoords[SubTextureIndex].Coords[3].x = std::roundf(AtlasCoords[SubTextureIndex].Coords[3].x * 1000.f) / 1000.f;

		AtlasCoords[SubTextureIndex].Coords[0].y = std::roundf(AtlasCoords[SubTextureIndex].Coords[0].y * 1000.f) / 1000.f;
		AtlasCoords[SubTextureIndex].Coords[1].y = std::roundf(AtlasCoords[SubTextureIndex].Coords[1].y * 1000.f) / 1000.f;
		AtlasCoords[SubTextureIndex].Coords[2].y = std::roundf(AtlasCoords[SubTextureIndex].Coords[2].y * 1000.f) / 1000.f;
		AtlasCoords[SubTextureIndex].Coords[3].y = std::roundf(AtlasCoords[SubTextureIndex].Coords[3].y * 1000.f) / 1000.f;


		
		FT_Glyph glyph{};
		FT_BBox box{};

		error = FT_Get_Glyph(slot, &glyph);
		if (error) {
			Core::Log(ErrorType::Error, "Failed to get glyph from slot.");
		}

		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &box);


		MinCord[SubTextureIndex] = {box.xMin/FontAtlasWidth,box.yMin/FontAtlasHeight};
		MaxCord[SubTextureIndex] = {box.xMax/FontAtlasWidth,box.yMax / FontAtlasHeight };




		
		SubTextureSizes[SubTextureIndex] = { (float)SizeX,(float)SizeY };

		for (uint32_t x = 0; x < slot->bitmap.width; x++) {	
			for (uint32_t y = 0; y < slot->bitmap.rows; y++) {
				AtlasMapBitmap[(uint32_t)OffsetX+x + ((y + (uint32_t)OffsetY)* (uint32_t)FontAtlasWidth )] |= uint32_t(slot->bitmap.buffer[(y * slot->bitmap.width+x)]<<24);
			}
		}
		//if (SubTextureIndex  == '.'-33)
		//	Core::Log(ErrorType::Error, "Maktyra");
		OffsetX += AtlasCoords[SubTextureIndex].Width+1;
		SubTextureIndex++;

		//Core::Log(ErrorType::Info,(char)i, "Left:", slot->bitmap_left, " Top:", slot->bitmap_top, " Diff:", -1*int((slot->metrics.height -slot->metrics.horiBearingY)/26));


	}
	//Core::Log(ErrorType::Error, "dwadad");
	Font* font = new Font();
	font->Coords = AtlasCoords;
	font->MinCord = MinCord;
	font->MaxCord = MaxCord;
	font->FontSize = m_CharacterSize;
	font->GlyphCount = m_Face->num_glyphs;
	font->TextureID = Core::GetStringHash("FontTexture");

	Texture* texture = new Texture(app->m_Renderer->GetContext(), FontAtlasWidth, FontAtlasHeight, 4, AtlasMapBitmap);

	app->m_AssetManager.LoadAsset(font, AssetType::FONT, "Font");
	app->m_AssetManager.LoadAsset(texture, AssetType::TEXTURE, "FontTexture");

	//m_FontAtlas->CreateTextureAtlas(AtlasCoords, SubTextureIndex,SubTextureSizes);
}
void FontSystem::PushStyle(const Style& style,void* StyleData) {
	m_Style.push(style);
	switch (style) {
	case Style::DrawBorder: {
		
		//Store style data somehow and transfer it.
			m_StyleData.push(new StyleBorderData());
			if (StyleData) {
				memcpy(m_StyleData.top(), StyleData, sizeof(StyleBorderData));
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
	delete m_StyleData.top();
	m_StyleData.pop();
	m_Style.pop();
}


