#pragma once
#include "AppCore.h"
#include "Event.h"
#define GUI_HEADER
class Application;
class GUIRenderer
{
public:
	GUIRenderer(Application* app);
	void BeginGUI();

	bool Button(Float2 Position, Float4 Color, Float2 Size,MouseCodes mousecode =MouseCodes::LEFT,bool SaveState=true,bool Dragable=false,bool** IsPressed=nullptr);


	void EndGUI();

private:
	Application* m_Application{};
	struct ButtonData {
		bool IsPressed{false};

		GUUID ID{};
	};
	std::unordered_map<uint32_t, ButtonData> m_ButtonIDs{};
	uint32_t m_CurrentButton{};

};

