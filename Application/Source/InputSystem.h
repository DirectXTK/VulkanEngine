#pragma once
#include "Event.h"
#include "Window.h"


struct InputCallbacks {
	void(*MouseButtonCallback)(MouseEvent*);
	void(*KeyBoardCallback)(KeyBoardEvent*);
	void(*WindowCallback)(WindowEvent*);

};
class InputSystem
{
public:
	void Init(GLFWwindow* window);

	void DispatchEventM(GLFWwindow* window,int Key,int Action,int Mods);
	void DispatchEventK(GLFWwindow* window,int Key,int ScanCode,int Action,int Mods);
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
	//Call back adding
	void AddCallbacks(InputCallbacks* callbacks);
private:
	bool m_KeyCodes[256];

	InputCallbacks m_Callbacks{};

	MouseEvent m_MouseEvents[7];

	Float2 m_MousePos{};
	bool m_MouseClick[3]{};
	bool m_MouseHold[3]{};

	Double2 m_MouseChange{};


	GLFWwindow* m_CurrentWindow{};

};