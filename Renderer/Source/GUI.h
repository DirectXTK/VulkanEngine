#pragma once
#include "AppCore.h"
#include "Event.h"
#define GUI_HEADER
class Application;
class GUIRenderer
{
public:
	GUIRenderer(Application* app,bool SaveState);
	void BeginGUI();

	void Panel(Float2 Position, Float4 Color, Float2 Size, GUUID TextureHandle = 0, bool Dragable = false);
	bool Button(Float2 Position, Float4 Color, Float2 Size,MouseCodes mousecode =MouseCodes::LEFT,GUUID TextureHandle=0,bool SaveState=true,bool Dragable=false,bool** IsPressed=nullptr);
	void EndPanel();

	void EndGUI();

private:
	Application* m_Application{};
	struct PanelData {
		Float2 Position{};
		Float2 Size{};
		GUUID ID{};
		PanelData* pParent{};
		Float2 ClickedPos{};
	};
	struct ButtonData {
		bool IsPressed{false};
		GUUID ID{};
		Float2 DragPosition{};
	};
	std::unordered_map<uint32_t, ButtonData> m_ButtonIDs{};
	std::unordered_map<uint32_t, PanelData> m_PanelIDs{};

	uint32_t m_PanelDepth{};
	PanelData* m_CurrenPanelParent{};
	uint32_t m_CurrentPanel{};
	uint32_t m_CurrentButton{};
	//If true when saves position of the buttons
	bool m_SaveState{};

};

