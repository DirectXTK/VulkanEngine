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
	Float2 GetWorldMousePos(Float2 CameraPosition, Float2 CameraScale, Float2 ViewportExtent);
	//Every frame
	void ResetInput();
private:
	bool m_KeyCodes[256];

	MouseEvent m_MouseEvents[7];

	Float2 m_MousePos{};
	bool m_MouseClick[3]{};
	bool m_MouseHold[3]{};


	GLFWwindow* m_CurrentWindow{};

};