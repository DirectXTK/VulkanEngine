#include "LogSystem.h"
namespace Core{
    void EmptyLogFile(){

		std::ofstream file("C:/Repos/Game/bin/windows/x86_64/Application/Log.txt");

        file.close();
	}
}