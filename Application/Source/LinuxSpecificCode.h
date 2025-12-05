#pragma once
#ifdef __linux__
#include <atomic>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <sys/stat.h>
namespace Core{
    bool ChechIfThereIsInputFromConsole();
    std::string ReadInputFromConsole(std::atomic<bool>& threadRunning);
}















#endif  