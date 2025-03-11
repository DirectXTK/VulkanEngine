#pragma once
#include "AppCore.h"
class InputSystem;
enum class WindowEventType{ShutDown,Resize};
enum class KeyCodes{A=0x41,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,PLUS= 0xBB,MINUS= 0xBD, ENTER =257,BACKSPACE =259, };
enum class MouseCodes {LEFT, RIGHT,SCROLL};
enum class KeyState {RELEASED,PRESSED,HOLD};

namespace Core {
	KeyCodes ConvertFromGlfwInt(int KeyCode, int Mod);
}
class KeyBoardEvent {
public:
	KeyCodes Key{};
	KeyState State{  }; 
};
class MouseEvent {
public:
	MouseCodes Code{};
	bool State{};
};
class WindowEvent {
public:

	WindowEventType Type{};
};
class EventHandler {
public:
	
	void DispatchEventK(KeyBoardEvent event, InputSystem* input);
	void DispatchEventW(WindowEvent event, InputSystem* input);
	void DispatchEventM(MouseEvent event, InputSystem* input);
	
private:
	//KeyBoard event buffer
	KeyBoardEvent m_KeyBoardEvent[24];
	uint32_t m_KeyBoardEventCount{};
	//Queue for the thread
	//count
	uint32_t m_DispatchedEvents{};
};
bool IsKeyPressed(KeyBoardEvent event, const KeyCodes& keycodes);