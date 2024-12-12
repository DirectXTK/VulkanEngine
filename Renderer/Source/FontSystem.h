#pragma once
#include "AppCore.h"
#include "Renderer.h"
#include "ft2build.h"
#include FT_FREETYPE_H
class FontSystem
{
public:
	FontSystem();
	void Run(void* app,void* Renderer);
	~FontSystem();
private:
	FT_Library m_Library{};
	FT_Face m_Face{};
	Texture* m_Texture{};
	Texture* m_Texture1{};

};

