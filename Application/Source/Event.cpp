#include "Event.h"
#include "InputSystem.h"

KeyCodes Core::ConvertFromGlfwInt(int KeyCode, int Mod)
{
    return (KeyCodes)KeyCode;
}
