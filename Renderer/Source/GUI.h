#pragma once
#include "AppCore.h"
#define GUI_HEADER
class Application;
class GUIRenderer
{
public:
	GUIRenderer(Application* app);
	void BeginGUI();

	bool Button(Float2 Position,Float4 Color,Float2 Size);

	void EndGUI();

private:
	Application* m_Application{};
	std::vector<GUUID> m_ButtonIDs{};
	uint32_t m_CurrentButton{};
};

