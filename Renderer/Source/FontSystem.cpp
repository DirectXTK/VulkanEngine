#include "FontSystem.h"
#include "Renderer.h"
#include "Application.h"

FontSystem* g_FontSystem{};
void MouseCallBackFn(MouseEvent* event) {
	g_FontSystem->MouseCallBack(event);
}
FontSystem::FontSystem(void* App)
{
	m_App = App;
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

	//ReRenderFaces();
	InputCallbacks callbacks{};
	callbacks.MouseButtonCallback = MouseCallBackFn;

	CurrentApp->AddCallback(&callbacks);

}

void FontSystem::Run(void* app,void* iRenderer)
{
	
}

void FontSystem::SetCharcterSize(float CharSize)
{
	m_CharacterSize = CharSize;
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

void FontSystem::InputText(const char* ID, char* Buffer,uint64_t BufferSize, Float2 Position, Float2 Size, uint32_t MaxCharacters)
{
	Application* app = (Application*)m_App;
	Renderer* renderer = ((Application*)m_App)->m_Renderer;
	bool ScrollableBoundBox{};


	m_WriteCooldown -= app->GetDeltaTime();
	m_PointerBlinking -= app->GetDeltaTime();

	Float2 BoundingBox[4];
	BoundingBox[0] = { Position.x ,Position.y  };
	BoundingBox[1] = { Position.x ,Position.y + Size.y };
	BoundingBox[2] = { Position.x + Size.x,Position.y + Size.y };
	BoundingBox[3] = { Position.x + Size.x,Position.y };
	

	//Editing of the buffer
	if (m_CharEditedIndex != -1) {
		if (m_WriteCooldown <= 0) {
			char PressedChar = app->m_InputSystem.GetWriteableKeyPressed();

			if (PressedChar != 0) {
				memcpy(Buffer+ m_CharEditedIndex+1, Buffer + m_CharEditedIndex , BufferSize - m_CharEditedIndex);

				Buffer[m_CharEditedIndex] = app->m_InputSystem.GetWriteableKeyPressed();
				m_CharEditedIndex++;
				Buffer[BufferSize - 1] = '\0';
				m_WriteCooldown = SEC(0.2f);
				m_PointerBlinking = SEC(0.6f);
				m_Show = true;
			}
		}
	}

	if (app->m_InputSystem.IsMouseClicked(MouseCodes::LEFT)&& app->GetCurrentlyHoveredPixelID() == Core::GetStringHash(ID).ID) {
		m_PointerLocation = app->GetMousePosNorm();
		m_IsPointerActive = true;
		m_CharEditedIndex = renderer->RenderText(Buffer, Position, BoundingBox, m_FixedPadding * m_CharacterSize, true, m_PointerLocation);

	}
	//Pointer blinking
	if (m_IsPointerActive) {
		if (m_PointerBlinking <= 0.0f) {
			if (m_Show) {
				m_Show = false;
				m_PointerBlinking = SEC(0.6f);

			}
			else {
				m_Show = true;
				m_PointerBlinking = SEC(0.6f);

			}
		}
		
	}

	 renderer->RenderText(Buffer, Position, BoundingBox, m_FixedPadding * m_CharacterSize, m_Show, m_PointerLocation,m_CharEditedIndex);

	

	//if (m_CharEditedIndex == -1)
		//Core::Log(ErrorType::Error, "Failed to find the char index.");
	//DrawPointer()
	// 
	//Draw the invisible barrier that  provides the selecting 
	renderer->DrawQuad({ Position.x + (Size.x * 0.5f),Position.y + (Size.y * 0.5f),0.0f }, { 1.0f,1.0f,1.0f,0.0f }, { Size.x * 0.5f,Size.y * 0.5f },Core::GetStringHash(ID).ID);

}

void FontSystem::PopFont()
{
}

void FontSystem::MouseCallBack(MouseEvent* event)
{
	Core::Log(ErrorType::Info, "Mouse button ", (uint32_t)event->Code, " Mouse state ", event->State);
}

FontSystem::~FontSystem()
{
}
void FontSystem::DrawPointer(Float2 Position, float CharacterSize,float SizeY)
{
	Application* app = (Application*)m_App;
	Renderer* renderer = app->m_Renderer;

	renderer->DrawQuad({ Position.x,Position.y }, { 1.0f,1.0f,1.0f,1.0f }, { m_FixedPadding * CharacterSize ,SizeY},0);
}
void FontSystem::ReRenderFaces()
{
	
	Application* app = (Application*)m_App;
	FT_Error error = FT_Set_Char_Size(m_Face, 0, m_CharacterSize, 500, 500);
	if (error) {
		Core::Log(ErrorType::Error, "Failed to set the font char size");
	}

	FT_GlyphSlot slot = m_Face->glyph;
	uint64_t FontAtlasWidth, FontAtlasHeight{};
	uint32_t SubTextureIndex{};
	uint32_t ChannelCount{};
	int64_t OffsetX{0};
	int64_t OffsetY{0};
	TextureAtlasCoords* AtlasCoords{};
	Float2* SubTextureSizes{};
	uint32_t SizeX{16};
	uint32_t SizeY{ 16 };
	int64_t MaxY{};

	FontAtlasWidth = m_Face->max_advance_width;
	FontAtlasHeight = m_Face->max_advance_height;

	AtlasCoords = new TextureAtlasCoords[m_Face->num_glyphs];
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
		
		AtlasCoords[SubTextureIndex].SizeX = slot->bitmap.width;
		AtlasCoords[SubTextureIndex].SizeY = slot->bitmap.rows;
		SizeX = slot->bitmap.width;
		SizeY = slot->bitmap.rows;

		if (OffsetX + AtlasCoords[SubTextureIndex].SizeX > FontAtlasWidth) {
			OffsetY += MaxY;
			MaxY = 0;
			OffsetX = 0;
		}
		MaxY = std::max(MaxY, (int64_t)slot->bitmap.rows);

		

		AtlasCoords[SubTextureIndex].Points[0] = { float(OffsetX / (float)FontAtlasWidth),float((SizeY+OffsetY) / (float)FontAtlasHeight) };
		AtlasCoords[SubTextureIndex].Points[1] = { float(OffsetX / (float)FontAtlasWidth),float(OffsetY / (float)FontAtlasHeight) };
		AtlasCoords[SubTextureIndex].Points[2] = { float((SizeX+OffsetX) / (float)FontAtlasWidth),float(OffsetY / (float)FontAtlasHeight) };
		AtlasCoords[SubTextureIndex].Points[3] = { float((SizeX+OffsetX) / (float)FontAtlasWidth) ,float((SizeY+OffsetY) / (float)FontAtlasHeight)  };

		SubTextureSizes[SubTextureIndex] = { (float)SizeX,(float)SizeY };

		for (uint32_t x = 0; x < slot->bitmap.width; x++) {
			for (uint32_t y = 0; y < slot->bitmap.rows; y++) {
				AtlasMapBitmap[OffsetX+x + ((y + OffsetY)* FontAtlasWidth )] |= uint32_t(slot->bitmap.buffer[(y * slot->bitmap.width+x)]<<24);
			}
		}
		OffsetX += AtlasCoords[SubTextureIndex].SizeX;
		SubTextureIndex++;
		
		


	}
	m_FontAtlas = new Texture(app->m_Renderer->GetContext(), FontAtlasWidth, FontAtlasHeight, 4, AtlasMapBitmap);
	m_FontAtlas->CreateTextureAtlas(AtlasCoords, SubTextureIndex,SubTextureSizes);

	delete[] AtlasCoords;
}


