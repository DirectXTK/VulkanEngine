#include "InputSystem.h"
#include "Application.h"
double scrollx,scrolly;
InputSystem* g_InputSystem{};
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollx=xoffset;
	scrolly=yoffset;
	
}
void WindowCloseCallback(GLFWwindow* Window) {
	WindowEvent event{};
	event.Type = WindowEventType::ShutDown;

	assert(g_InputSystem!=nullptr);
	g_InputSystem->DispatchEventW(event);
}
void KeyCallBack(GLFWwindow* window,int Key,int ScanCode,int action,int mods){

	assert(g_InputSystem != nullptr);
	g_InputSystem->DispatchEventK(window, Key, ScanCode, action, mods);
}

void MouseButtonCallBack(GLFWwindow* window, int Key, int Action, int Mod) {
	assert(g_InputSystem != nullptr);
	g_InputSystem->DispatchEventM(window, Key, Action, Mod);
}
void InputSystem::Init(GLFWwindow* window){
			m_CurrentWindow = window;
			g_InputSystem = this;
			//glfwSetInputMode(m_CurrentWindow,GLFW_STICKY_MOUSE_BUTTONS,1);
	glfwSetScrollCallback(m_CurrentWindow,scroll_callback);
	glfwSetMouseButtonCallback(m_CurrentWindow, MouseButtonCallBack);
	glfwSetKeyCallback(m_CurrentWindow, KeyCallBack);
	glfwSetWindowCloseCallback(m_CurrentWindow, WindowCloseCallback);
}

void InputSystem::DispatchEventM(GLFWwindow* window, int Key, int Action, int Mods)
{
	MouseEvent Event{};
	Event.Code = (MouseCodes)Key;
	Event.State = Action;
	if (m_Callbacks.MouseButtonCallback)
		m_Callbacks.MouseButtonCallback(&Event);

}
void InputSystem::DispatchEventK(GLFWwindow* window, int Key, int ScanCode, int Action, int Mods)
{
	KeyBoardEvent Event{};
	Event.Key = (KeyCodes)Key;
	Event.State = (KeyState)Action;
	if(m_Callbacks.KeyBoardCallback)
		m_Callbacks.KeyBoardCallback(&Event);
}
float InputSystem::GetScroll(){
	return (float)scrolly;
}

void InputSystem::DispatchEventW(WindowEvent& event)
{
	if (m_Callbacks.WindowCallback)
		m_Callbacks.WindowCallback(&event);
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
char InputSystem::GetWriteableKeyPressed() {

	//33 is the first character by unicode and 126 is the last	
		for (uint32_t i = 33; i < 126; i++) {
			if (glfwGetKey(m_CurrentWindow, i) == GLFW_PRESS)
				return i;

		}

		return 0;
}
	bool IsMouseClicked(int Button);

Float2 InputSystem::GetMousePos()
{
	double x,y{};

	glfwGetCursorPos(m_CurrentWindow,&x,&y);
	return {(float)x,(float)y};
}
Float2 InputSystem::GetWorldMousePos(Float2 CameraPosition, Float2 CameraScale,Float2 ViewportExtent)
{
	Float2 MousePos = GetMousePos();

	//std::cout << "X:" << MousePos.x << " Y:" << MousePos.y << "\n";

	MousePos.x = (MousePos.x - (ViewportExtent.x*0.5f))/ (ViewportExtent.x*0.5f);
	MousePos.y = (1.0f-(MousePos.y - (ViewportExtent.y*0.5f)))/ (ViewportExtent.y*0.5f);

	MousePos.x /= CameraScale.x;
	MousePos.y /= CameraScale.y;
	MousePos.x += CameraPosition.x ;
	MousePos.y += CameraPosition.y ;




	return MousePos;
}

Float2 InputSystem::GetMousePosChange()
{

	return { (float)m_MouseChange.x,(float)m_MouseChange.y };
}

bool InputSystem::IsMouseClicked(MouseCodes codes,bool Hold)
{
	return glfwGetMouseButton(m_CurrentWindow,(int)codes)&& (!m_MouseHold[(int)codes]||Hold);
}



void InputSystem::ResetMouseChange()
{
	int Width, Height{};
	double x, y{};
	glfwGetWindowSize(m_CurrentWindow, &Width, &Height);
	glfwGetCursorPos(m_CurrentWindow, &x, &y);

	m_MouseChange.x = float(x - m_MouseChange.x) ;
	m_MouseChange.y = float(y - m_MouseChange.y);

}

void InputSystem::ResetInput()
{
	scrolly = 0;
	scrollx = 0;

	m_MouseClick[0] = glfwGetMouseButton(m_CurrentWindow, (int)MouseCodes::LEFT);
	m_MouseClick[1] = glfwGetMouseButton(m_CurrentWindow, (int)MouseCodes::RIGHT);
	m_MouseClick[2] = glfwGetMouseButton(m_CurrentWindow, (int)MouseCodes::SCROLL);

	if (m_MouseClick[0])
		m_MouseHold[0] = true;
	else
		m_MouseHold[0] = false;


	if (m_MouseClick[1])
		m_MouseHold[1] = true;
	else
		m_MouseHold[1] = false;


	if (m_MouseClick[2])
		m_MouseHold[2] = true;
	else
		m_MouseHold[2] = false;

	
	double x, y{};
	glfwGetCursorPos(m_CurrentWindow, &x, &y);
	m_MouseChange.x = x;
	m_MouseChange.y = y;

	

}

void InputSystem::AddCallbacks(InputCallbacks* callbacks)
{
	m_Callbacks = *callbacks;
}

