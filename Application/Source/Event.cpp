#include "Event.h"
#include "InputSystem.h"

KeyCodes Core::ConvertFromGlfwInt(int KeyCode, int Mod)
{
    Core::Log(ErrorType::Info, KeyCode, " ", Mod);
    return (KeyCodes)KeyCode;
}
