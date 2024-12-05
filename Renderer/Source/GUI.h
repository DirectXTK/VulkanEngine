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

	void Panel(const std::string& ID,Float2 Position, Float4 Color, Float2 Size, GUUID TextureHandle = 0, bool Dragable = false);
	bool Button(const std::string& ID,Float2 Position, Float4 Color, Float2 Size,MouseCodes mousecode =MouseCodes::LEFT,GUUID TextureHandle=0,bool SaveState=true,bool Dragable=false,bool** IsPressed=nullptr);
	void EndPanel();

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
		bool IsPressed{false};
		Float2 DragPosition{};
	};
	std::unordered_map<std::string, ButtonData> m_Buttons{};
	std::unordered_map<std::string, PanelData> m_Panels{};

	//std::unordered_map<uint32_t, ButtonData> m_ButtonIDs{};
	std::unordered_map<uint32_t, PanelData> m_PanelIDs{};

	uint32_t m_PanelDepth{};
	PanelData* m_CurrenPanelParent{};
	uint32_t m_CurrentPanel{};
	int32_t m_DraggedPanel{-1};
	Float2 m_DraggedPanelDragAmount{};
	//uint32_t m_CurrentButton{};
	//If true when saves position of the buttons
	bool m_SaveState{};

};

