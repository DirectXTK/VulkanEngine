#pragma once
#ifdef WINDOWS
#include "AppCore.h"
namespace Core{
    std::string ReadInputFromConsole(std::atomic<bool>& threadRunning);

}


 #endif