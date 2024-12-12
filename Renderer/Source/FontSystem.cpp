#include "FontSystem.h"
#include "Renderer.h"
#include "Application.h"
FontSystem::FontSystem()
{
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
	error = FT_Set_Char_Size(m_Face, 0, 16*64,250, 250);
	if (error) {
		Core::Log(ErrorType::Error, "Failed to set the font char size");
	}

}

void FontSystem::Run(void* app,void* iRenderer)
{
	Renderer* renderer = (Renderer*)iRenderer;
	Application* ap = (Application*)app;

	FT_GlyphSlot slot = m_Face->glyph;

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


}

FontSystem::~FontSystem()
{
}

