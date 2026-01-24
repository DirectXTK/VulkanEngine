#pragma once
#include "GUIStyles.h"
#include "ft2build.h"
#include "Texture.h"
#include "AssetManager.h"
#include "Event.h"

#include FT_FREETYPE_H
class Application;
class Renderer;

struct Font {
	Asset<Texture> TextureAsset{};
	//the first uint32_t is the char code in unicode the second is the fonts index to texture.
	std::unordered_map<uint32_t,uint32_t> CharMap{};
	std::string FontName{"NONE"};
	GUUID TextureID{};
	uint32_t FontSize{};
	Float2* Advance{};
	TextureCoords* Coords{};
	Float2* MinCord{};
	Float2* MaxCord{};
	uint32_t GlyphCount{};
	~Font(){
		delete[] Coords;
		delete[] MinCord;
		delete[] MaxCord;
		delete[] Advance;
		Core::Log("Font data freed");
	}
};

class FontSystem
{
public:
	FontSystem();
	void Run(void* app,void* Renderer);

	void SetCharcterSize(uint32_t CharSize);

	uint32_t GetWidthOfChar();
	uint32_t GetHeightOfChar();

	Texture* GetFontAtlas();

	void PushStyle(const GUI::Style& style,void* StyleData=nullptr);
	void PopStyle();


	void PushFont(GUUID fontAsset);
	//Renders simple text.
	// MaxCharacters 0 means unlimited.
	//Returns true then max characters has been reached.
	void InputText(const char* ID,char* Buffer,uint64_t BufferSize,Float2 Position,Float2 Size);


	void Text(const char* StrID,const char* Message, Float2 Position, Float2 MaxSize = {0.f,0.f});
	void Text(GUUID id, const char* Message, Float2 Position, Float2 MaxSize = { 0.f,0.f });

	uint32_t GetFontSize() { return m_CharacterSize;}
	void PopFont();

	void KeyBoardCallback(KeyBoardEvent* event);
	Asset<Font> LoadFont(const std::string& filePath);

	~FontSystem();
private:
	//Character size is used for the pointer size

	void SpecialCases(KeyCodes& Code, EventState& State,char* Buffer,uint64_t Size);
	int64_t CalculateCharBeingEditedIndex();
	void DrawPointer(Float2 Position,float CharacterSize,float SizeY);
	void DrawBorder(Float2& Position, Float2& Size, GUUID ID);


	//coeficient used for normalizing char size.
	const float m_CharSizeNormCoe{ 0.000043f };

	Renderer* m_Renderer{};
	Float2 m_TextureSize{};
	Asset<Font> ReRenderFaces(GUUID fontID,const std::string& fontName);
	FT_Library m_Library{};

	uint32_t m_FaceCount{};
	FT_Face m_Face{};

	Texture* m_FontAtlas{};
	Texture* m_Texture1{};
	float m_Padding{ 0.1f };
	float m_PaddingY{0.1f};
	uint32_t m_CharacterSize{1};

	uint32_t m_FontAtlasSize{};
	Texture* m_FontTexture{};
	//Pointer
	//Typing
	float m_TypingCooldown{};

	std::queue<EventState> m_KeyStates{};
	std::queue<KeyCodes> m_KeyCodes{};
	bool m_KeyAlreadyPressed[300];
	

	//styles
	std::stack<GUI::Style> m_Style{};
	std::stack<void*> m_StyleData{};


	const float m_CharEditCooldownConst{SEC(0.15f)};
	const float m_DeleteCharCooldownConst{ m_CharEditCooldownConst };
	const float m_PointerBlinkCooldownConst{ SEC(0.5f) };
	float m_DeleteCharCooldown{};

	Float2 m_PointerLocation{};
	bool m_IsPointerActive{false};
	int64_t m_CharEditedIndex{-1};
	float m_PointerCooldown{0.0f};


	struct TextData {
		const char* Message{};
	};
	//Stored data
	std::stack<Asset<Font>> m_FontAssets;
	std::unordered_map<GUUID, TextData> m_StoredData{};
	//float m_FixedPadding{ 0.1018f  };

};

