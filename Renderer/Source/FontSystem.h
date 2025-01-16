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

	void PushFont();
	//Renders simple text.
	// MaxCharacters 0 means unlimited.
	//Returns true then max characters has been reached.
	void InputText(const char* ID,char* Buffer,uint64_t BufferSize,Float2 Position,Float2 Size,uint32_t MaxCharacters=0);
	void Text(const char* Message, Float2 Position);

	void PopFont();

	~FontSystem();
private:
	//Character size is used for the pointer size
	void DrawPointer(Float2 Position,float CharacterSize,float SizeY);

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
	//Pointer
	Float2 m_PointerLocation{};
	bool m_IsPointerActive{false};
	int64_t m_CharEditedIndex{-1};
	float m_WriteCooldown{0.0f};

	struct TextData {
		const char* Message{};
	};
	//Stored data
	std::unordered_map<GUUID, TextData> m_StoredData{};
	float m_FixedPadding{ 0.00002f  };

};

