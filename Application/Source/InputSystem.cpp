#include "InputSystem.h"
#include "Application.h"

#ifdef WINDOWS
#include <Window.h>
#endif
#ifdef LINUX
#include <xkbcommon/xkbcommon.h>
struct Keyboard {
    xkb_context* ctx;
    xkb_keymap* keymap;
    xkb_state* state;
};
void HandleKey();
#endif

Keyboard g_Keyboard{};
void InitKeyboard(){

    g_Keyboard.ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    g_Keyboard.keymap = xkb_keymap_new_from_names(
        g_Keyboard.ctx,
        nullptr,  // use system layout
        XKB_KEYMAP_COMPILE_NO_FLAGS
    );
    g_Keyboard.state = xkb_state_new(g_Keyboard.keymap);

}
double scrollx,scrolly;
InputSystem* g_InputSystem{};
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollx=xoffset;
	scrolly=yoffset;
	
}
void WindowCloseCallback(GLFWwindow* Window) {
	WindowShutDownEvent event{};
	assert(g_InputSystem!=nullptr);
	g_InputSystem->DispatchEvent(event);
}
void KeyCallBack(GLFWwindow* window,int Key,int ScanCode,int action,int mods){
	KeyBoardEvent event{};
	event.Key = (KeyCodes)Key;
	event.State = (EventState)action;
	assert(g_InputSystem != nullptr);
	g_InputSystem->DispatchEvent(event);

	TextEvent textEvent{};
	textEvent.KeyUint = ScanCode;
	textEvent.State = (EventState)action;

	xkb_state_update_key(
        g_Keyboard.state,
        textEvent.KeyUint,
        action != GLFW_RELEASE
            ? XKB_KEY_DOWN
            : XKB_KEY_UP
    );

    if (action == GLFW_RELEASE )
        return;

    char buf[2];
    int len = xkb_state_key_get_utf8(
    g_Keyboard.state, (xkb_keycode_t)textEvent.KeyUint, buf, ARRAYSIZE(buf));
	if(len > 0){
		textEvent.KeyUint = buf[0];
		if(textEvent.KeyUint >15)
			g_InputSystem->DispatchEvent(textEvent);
	}
}
void ScrollCallback(GLFWwindow* window,double offsetX,double offsetY){
	MouseEvent event{};
	event.Code = MouseCodes::SCROLL;
	event.ScrollY = offsetY;
	g_InputSystem->DispatchEvent(event);
}
void MouseButtonCallBack(GLFWwindow* window, int Key, int Action, int Mod) {
	MouseEvent event{};
	event.Code = (MouseCodes)Key;
	event.State = (EventState)Action;
	assert(g_InputSystem != nullptr);
	g_InputSystem->DispatchEvent(event);
}
void WindowResizeCallback(GLFWwindow* window,int width,int height){
	WindowResizeEvent event{};
	event.Width = width;
	event.Height = height;
	assert(g_InputSystem!= nullptr);
	Application::GetRenderer()->OnWindowResize(width,height);
	g_InputSystem->DispatchEvent(event);
}
void InputSystem::Init(GLFWwindow* window){
			m_CurrentWindow = window;
			g_InputSystem = this;
			//glfwSetInputMode(m_CurrentWindow,GLFW_STICKY_MOUSE_BUTTONS,1);
	glfwSetScrollCallback(m_CurrentWindow,scroll_callback);
	glfwSetMouseButtonCallback(m_CurrentWindow, MouseButtonCallBack);
	glfwSetKeyCallback(m_CurrentWindow, KeyCallBack);
	glfwSetWindowCloseCallback(m_CurrentWindow, WindowCloseCallback);
	glfwSetWindowSizeCallback(m_CurrentWindow,WindowResizeCallback);
	glfwSetScrollCallback(m_CurrentWindow,ScrollCallback);

	InitKeyboard();
}
void InputSystem::DispatchEvent(Event& event){
	Application::DispatchEvent(event);
}
float InputSystem::GetScroll(){
	return (float)scrolly;
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
	m_MouseChange.x = float(m_LastFrameMousePos.x -x) ;
	m_MouseChange.y = float(m_LastFrameMousePos.y -y);

	glfwGetCursorPos(m_CurrentWindow, &x, &y);
	m_LastFrameMousePos.x = x;
	m_LastFrameMousePos.y = y;

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

	
	

	

}
InputSystem::~InputSystem(){
	if(g_Keyboard.state)
		xkb_state_unref(g_Keyboard.state);
	if(g_Keyboard.keymap)
		xkb_keymap_unref(g_Keyboard.keymap);
	if(g_Keyboard.ctx)
	 	xkb_context_unref(g_Keyboard.ctx);


	g_Keyboard.state = nullptr;
	g_Keyboard.keymap = nullptr;
	g_Keyboard.ctx = nullptr;
}