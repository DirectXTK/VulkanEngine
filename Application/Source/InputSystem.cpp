#include "InputSystem.h"
#include "Application.h"
double scrollx,scrolly;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollx=xoffset;
	scrolly=yoffset;
	
}
	void InputSystem::Init(GLFWwindow* window){
			m_CurrentWindow = window;
			glfwSetInputMode(m_CurrentWindow,GLFW_STICKY_MOUSE_BUTTONS,1);
	glfwSetScrollCallback(m_CurrentWindow,scroll_callback);

	}

void InputSystem::DispatchEventM(MouseEvent& event)
{
	
}
float InputSystem::GetScroll(){
	return scrolly;
}

void InputSystem::DispatchEventK(KeyBoardEvent& event)
{
	
}

void InputSystem::DispatchEventW(WindowEvent& event)
{
}

bool InputSystem::IsKeyPressed(KeyCodes keycode)
{
	
	auto state =  glfwGetKey(m_CurrentWindow,(int)keycode);
	return state == GLFW_PRESS;
}

bool InputSystem::IsKeyReleased(KeyCodes keycode)
{
	
	auto state =  glfwGetKey(m_CurrentWindow,(int)keycode);
	return state == GLFW_RELEASE;
}

	bool IsMouseClicked(int Button);

Float2 InputSystem::GetMousePos()
{
	double x,y{};

	glfwGetCursorPos(m_CurrentWindow,&x,&y);
	return {(float)x,(float)y};
}
bool InputSystem::IsMouseClicked(MouseCodes codes)
{
	return glfwGetMouseButton(m_CurrentWindow,(int)codes);
}

void InputSystem::ResetInput()
{
		scrolly=0;
		scrollx =0;
}
