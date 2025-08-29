#include "LinuxSpecificCode.h"
#include <cstdint>
#include "Application.h"
namespace Core{

    char GetCharFromConsole(int64_t* size){
        termios oldt,newt{};
        char ch{};



        tcgetattr(STDIN_FILENO,&oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO,TCSANOW,&newt);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO,F_SETFL,flags|O_NONBLOCK);

       *size =  read(STDIN_FILENO,&ch,1);
        tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
        return ch;
    }
    void DeleteChar(){
            std::cout << "\b \b" << std::flush;
    }
    std::string ReadInputFromConsole(std::atomic<bool>& threadRunning){
        char ch{};
        int64_t Size{};
        std::string ret{};
        while(threadRunning.load()){
            
                ch = GetCharFromConsole(&Size);
                if(Size ==-1){
                    continue;
                }
                std::cout <<ch<<std::flush;
                if(ch == 127){
                    if(!ret.empty())
                    ret.pop_back();
                   DeleteChar();
                    continue;
                }
                else if(ch == '\n'){
                    return ret;
                }
                ret += ch;
                

    }
    return "";
}
}
   
