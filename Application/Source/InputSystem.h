#pragma once
#include "Event.h"
#include "Window.h"
class InputSystem
{
public:
	void Init(GLFWwindow* window);

	void DispatchEventM(MouseEvent& event);
	void DispatchEventK(KeyBoardEvent& event);
	void DispatchEventW(WindowEvent& event);

	bool IsKeyPressed(KeyCodes keycode);
	bool IsKeyReleased(KeyCodes keycode);

	bool IsMouseClicked(MouseCodes codes);
	float GetScroll();
	Float2 GetMousePos();
	//Every frame
	void ResetInput();
private:
	bool m_KeyCodes[256];

	MouseEvent m_MouseEvents[7];
	Float2 m_MousePos{};

	GLFWwindow* m_CurrentWindow{};

};