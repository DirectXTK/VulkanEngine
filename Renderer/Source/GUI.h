#pragma once
#include "AppCore.h"
#include "Event.h"
#define GUI_HEADER
class Application;
class FontSystem;
class GUIRenderer
{
public:
	GUIRenderer(Application* app, bool SaveState);
	void BeginGUI();

	void Panel(const std::string& ID, Float2 Position, Float4 Color, Float2 Size, GUUID TextureHandle = 0, bool Dragable = false);
	bool Button(const std::string& ID, const std::string& Text, Float2 Position, Float4 Color, Float2 Size, MouseCodes mousecode = MouseCodes::LEFT, GUUID TextureHandle = 0, bool SaveState = true, bool Dragable = false, bool** IsPressed = nullptr);

	void Text(const std::string& strID, const std::string& Text, Float2 Position, Float4 Color, Float2 Size);

	void Slider(const std::string& strID, float* number, Float2 Position,  Float2 Size, float SlideAmount=0.01f,uint32_t DecimalPlaces=3);
	void Slider(const std::string& strID, int* number, Float2 Position, Float2 Size, float SlideAmount = 0.01f);

	void InputText(const char* ID, char* Buffer, uint64_t BufferSize, Float2 Position, Float2 Size);

	void EndPanel();



	void SetFontSize(float Size);
	float GetFontSize();

	void EndGUI();

private:
	Application* m_Application{};
	struct PanelData {
		Float2 Position{};
		Float2 Size{};
		GUUID ID{};
		PanelData* pParent{};
		Float2 Offset{};
	};
	struct ButtonData {
		bool IsPressed{ false };
		Float2 DragPosition{};
	};
	struct SliderData {
		bool IsClicked{false};
	};
	std::unordered_map<std::string, ButtonData> m_Buttons{};
	std::unordered_map<std::string, PanelData> m_Panels{};
	std::unordered_map<std::string, SliderData> m_Sliders{};


	//std::unordered_map<uint32_t, ButtonData> m_ButtonIDs{};
	std::unordered_map<uint32_t, PanelData> m_PanelIDs{};

	uint32_t m_PanelDepth{};
	PanelData* m_CurrenPanelParent{};
	uint32_t m_CurrentPanel{};
	int32_t m_DraggedPanel{ -1 };
	Float2 m_DraggedPanelDragAmount{};
	//uint32_t m_CurrentButton{};
	//If true when saves position of the buttons
	bool m_SaveState{};

	FontSystem* m_FontSystem{};
};

