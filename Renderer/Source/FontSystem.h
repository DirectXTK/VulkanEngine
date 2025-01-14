#pragma once
#include "AppCore.h"
#include "Renderer.h"
#include "ft2build.h"
#include FT_FREETYPE_H
class FontSystem
{
public:
	FontSystem(void* app);
	void Run(void* app,void* Renderer);

	void SetCharcterSize(float CharSize);

	uint32_t GetWidthOfChar();
	uint32_t GetHeightOfChar();

	Texture* GetFontAtlas();

	~FontSystem();
private:
	void* m_App{};
	void ReRenderFaces();
	FT_Library m_Library{};

	uint32_t m_FaceCount{};
	FT_Face m_Face{};

	Texture* m_FontAtlas{};
	Texture* m_Texture1{};
	float m_CharacterSize{16*64};

	uint32_t m_FontAtlasSize{};
	Texture* m_FontTexture{};

};

