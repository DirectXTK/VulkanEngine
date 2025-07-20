#pragma once
#include "FontStyles.h"
#include "ft2build.h"
#include "Texture.h"
#include "AssetManager.h"
#include "Event.h"

#include FT_FREETYPE_H
class Application;
class Renderer;

struct Font {
	Asset<Texture> TextureAsset{};
	GUUID TextureID{};
	uint32_t FontSize{};
	TextureCoords* Coords{};
	Float2* MinCord{};
	Float2* MaxCord{};
	uint32_t GlyphCount{};
	~Font(){
		delete[] Coords;
		delete[] MinCord;
		delete[] MaxCord;
	}
};

class FontSystem
{
public:
	FontSystem(Application* app);
	void Run(void* app,void* Renderer);

	void SetCharcterSize(uint32_t CharSize);

	uint32_t GetWidthOfChar();
	uint32_t GetHeightOfChar();

	Texture* GetFontAtlas();

	void PushStyle(const Style& style,void* StyleData=nullptr);
	void PopStyle();


	void PushFont();
	//Renders simple text.
	// MaxCharacters 0 means unlimited.
	//Returns true then max characters has been reached.
	void InputText(const char* ID,char* Buffer,uint64_t BufferSize,Float2 Position,Float2 Size);


	void Text(const char* StrID,const char* Message, Float2 Position, Float2 MaxSize = {0.f,0.f});
	void Text(GUUID id, const char* Message, Float2 Position, Float2 MaxSize = { 0.f,0.f });

	uint32_t GetFontSize() { return m_CharacterSize;}
	void PopFont();

	void KeyBoardCallback(KeyBoardEvent* event);

	~FontSystem();
private:
	//Character size is used for the pointer size

	void SpecialCases(KeyCodes& Code, KeyState& State,char* Buffer,uint64_t Size);
	int64_t CalculateCharBeingEditedIndex();
	void DrawPointer(Float2 Position,float CharacterSize,float SizeY);
	void DrawBorder(Float2& Position, Float2& Size, GUUID ID);

	//coeficient used for normalizing char size.
	const float m_CharSizeNormCoe{ 0.000043f };

	Renderer* m_Renderer{};
	Application* m_App{};
	Float2 m_TextureSize{};
	void ReRenderFaces();
	FT_Library m_Library{};

	uint32_t m_FaceCount{};
	FT_Face m_Face{};

	Texture* m_FontAtlas{};
	Texture* m_Texture1{};
	float m_Padding{ 0.1f };
	float m_PaddingY{0.1f};
	uint32_t m_CharacterSize{46};

	uint32_t m_FontAtlasSize{};
	Texture* m_FontTexture{};
	//Pointer
	//Typing
	float m_TypingCooldown{};

	std::queue<KeyState> m_KeyStates{};
	std::queue<KeyCodes> m_KeyCodes{};
	bool m_KeyAlreadyPressed[300];
	

	//styles
	std::stack<Style> m_Style{};
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
	std::unordered_map<GUUID, TextData> m_StoredData{};
	//float m_FixedPadding{ 0.1018f  };

};

