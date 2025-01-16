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
	//if this returns 0 means no keys are pressed.
	char GetWriteableKeyPressed();

	bool IsMouseClicked(MouseCodes codes,bool Hold = false);

	float GetScroll();
	Float2 GetMousePos();
	Float2 GetWorldMousePos(Float2 CameraPosition, Float2 CameraScale, Float2 ViewportExtent);
	Float2 GetMousePosChange();
	//Every frame
	void ResetMouseChange();
	void ResetInput();
private:
	bool m_KeyCodes[256];

	MouseEvent m_MouseEvents[7];

	Float2 m_MousePos{};
	bool m_MouseClick[3]{};
	bool m_MouseHold[3]{};

	Double2 m_MouseChange{};


	GLFWwindow* m_CurrentWindow{};

};