#pragma once
#include "AppCore.h"
class InputSystem;
enum class KeyCodes{NUM0=48,NUM1,NUM2,NUM3,NUM4,NUM5,NUM6,NUM7,NUM8,NUM9,
	A=0x41,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,PLUS= 0xBB,MINUS= 0xBD,ARROWRIGHT = 262, ARROWLEFT,ARROWDOWN, ARROWUP, ENTER =257,BACKSPACE =259 };
enum class MouseCodes {LEFT, RIGHT,SCROLL};
//Expended means this thas expended by other function.
enum class EventState  {RELEASED,PRESSED,HOLD};
enum class EventType{NONE,EXPENDED,KEYBOARD,MOUSE,WINDOWSHUTDOWN,WINDOWRESIZE};
namespace Core {
	KeyCodes ConvertFromGlfwInt(int KeyCode, int Mod);
}
class Event{
	public:
		Event(const EventType& type): m_EventType(type){}
		EventType GetEventType()const {return m_EventType;}
		void Expend(){m_EventType = EventType::EXPENDED;}
	private:
		EventType m_EventType{EventType::NONE};
};
class KeyBoardEvent:public Event {
public:
	KeyBoardEvent():Event(EventType::KEYBOARD){}
	KeyCodes Key{};
	EventState State{  }; 
};
class MouseEvent:public Event {
public:
	MouseEvent():Event(EventType::MOUSE){}


	MouseCodes Code{};
	EventState State{};
};
class WindowShutDownEvent :public Event{
public:
	WindowShutDownEvent():Event(EventType::WINDOWSHUTDOWN){}

};
class WindowResizeEvent :public Event{
public:
	WindowResizeEvent():Event(EventType::WINDOWRESIZE){}
	int Width{},Height{};
};
class EventHandler {
public:
	
	void DispatchEventK(KeyBoardEvent event, InputSystem* input);
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