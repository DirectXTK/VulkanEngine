#include "Event.h"
#include "InputSystem.h"
bool IsKeyPressed(KeyBoardEvent event, const KeyCodes& keycodes)
{
	if (event.Key == keycodes && event.Pressed == true)
		return true;
	return false;
}

void EventHandler::DispatchEventK(KeyBoardEvent event, InputSystem* input)
{
	input->DispatchEventK(event);
}

void EventHandler::DispatchEventW(WindowEvent event, InputSystem* input)
{
	input->DispatchEventW(event);
}

void EventHandler::DispatchEventM(MouseEvent event, InputSystem* input)
{
	input->DispatchEventM(event);
}