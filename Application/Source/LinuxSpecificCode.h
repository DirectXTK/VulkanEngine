#pragma once
#define LINUX
#ifdef LINUX
#include <atomic>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
namespace Core{
    bool ChechIfThereIsInputFromConsole();
    std::string ReadInputFromConsole(std::atomic<bool>& threadRunning);
}















#endif  