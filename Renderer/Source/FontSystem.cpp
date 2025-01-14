#include "FontSystem.h"
#include "Renderer.h"
#include "Application.h"
FontSystem::FontSystem(void* App)
{
	m_App = App;
	const char* FontPath = "C:\\Repos\\VulkanEngine\\Resources\\Fonts\\Daydream.ttf";

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
	else {
		Core::Log(ErrorType::Error, "FaceCount:", m_Face->num_glyphs);
		
	}
	//ReRenderFaces();
	

}

void FontSystem::Run(void* app,void* iRenderer)
{
	/*
	Renderer* renderer = (Renderer*)iRenderer;
	Application* ap = (Application*)app;
	FT_GlyphSlot slot = m_Face->glyph;
	m_Face.gl

	uint32_t GlyphIndex = FT_Get_Char_Index(m_Face, 'a');

	FT_Error error=FT_Load_Glyph(m_Face, GlyphIndex, FT_LOAD_DEFAULT);
	if (error)
		Core::Log(ErrorType::Error, "Failed to load glyph");

	 error = FT_Render_Glyph(slot,FT_RENDER_MODE_NORMAL);
	if (error)
		Core::Log(ErrorType::Error, "Failed to render glyph");

	static bool Init{false};
	if (!Init) {
	 m_Texture= new Texture(renderer->GetContext(),slot->bitmap.width,slot->bitmap.pitch, 4,(uint32_t*)slot->bitmap.buffer);
	 Init = true;
	}
	


	GUUID TextId = ap->GetAssetManager()->LoadResource(m_Texture, ResourceType::TEXTURE, "TestingFont");

	

	 GlyphIndex = FT_Get_Char_Index(m_Face, 'u');

	 error = FT_Load_Glyph(m_Face, GlyphIndex, FT_LOAD_DEFAULT);
	if (error)
		Core::Log(ErrorType::Error, "Failed to load glyph");

	error = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
	if (error)
		Core::Log(ErrorType::Error, "Failed to render glyph");

	static bool Init1{ false };
	if (!Init1) {
		m_Texture1 = new Texture(renderer->GetContext(), slot->bitmap.width, slot->bitmap.pitch, 4, (uint32_t*)slot->bitmap.buffer);
		Init1 = true;
	}

	GUUID TextId1 = ap->GetAssetManager()->LoadResource(m_Texture1, ResourceType::TEXTURE, "TestingFont1");
	renderer->DrawQuad({ 0.5f,0.5f,0.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 0.1f,0.1f }, TextId, 0);


	renderer->DrawQuad({ 0.71f,0.5f,0.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 0.1f,0.1f }, TextId1, 0);
	*/

}

void FontSystem::SetCharcterSize(float CharSize)
{
	m_CharacterSize = CharSize*16;
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

FontSystem::~FontSystem()
{
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
	uint32_t SizeX{16};
	uint32_t SizeY{ 16 };
	int64_t MaxY{};

	FontAtlasWidth = m_Face->max_advance_width;
	FontAtlasHeight = m_Face->max_advance_height;

	AtlasCoords = new TextureAtlasCoords[m_Face->num_glyphs];

	error = FT_Select_Charmap(m_Face, FT_ENCODING_UNICODE);
	if (error)
		Core::Log(ErrorType::Error, "Failed to select charmap.");

	uint32_t* AtlasMapBitmap = new uint32_t[FontAtlasWidth * FontAtlasHeight];
	memset(AtlasMapBitmap,0x00000000, FontAtlasWidth * FontAtlasHeight*sizeof(uint32_t));

	for (uint32_t i = 0; i < m_Face->num_glyphs; i++) {
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


		for (uint32_t x = 0; x < slot->bitmap.width; x++) {
			for (uint32_t y = 0; y < slot->bitmap.rows; y++) {
				AtlasMapBitmap[OffsetX+x + ((y + OffsetY)* FontAtlasWidth )] |= uint32_t(slot->bitmap.buffer[(y * slot->bitmap.width+x)]<<24);
			}
		}
		OffsetX += AtlasCoords[SubTextureIndex].SizeX;
		SubTextureIndex++;
		Core::Log(ErrorType::Info, "PixelMode ", slot->format);
		if (FT_GLYPH_FORMAT_BITMAP == slot->format) {
			Core::Log(ErrorType::Info, "IsBitmap");
		}
		


	}
	m_FontAtlas = new Texture(app->m_Renderer->GetContext(), FontAtlasWidth, FontAtlasHeight, 4, AtlasMapBitmap);
	m_FontAtlas->CreateTextureAtlas(AtlasCoords, SubTextureIndex);

	delete[] AtlasCoords;
}


