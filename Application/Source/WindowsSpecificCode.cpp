#include "WindowSpecificCode.h"
#ifdef WINDOWS
#endif
namespace Core{

    std::string ReadInputFromConsole(std::atomic<bool>& threadRunning){
        std::string temp{};
        std::cin >> temp;

        return temp;
    }
}




