#include "Window.h"
 Window::Window(uint32_t Width,uint32_t Height,std::string name){

  
       
        m_Window = glfwCreateWindow(Width,Height,name.c_str(),nullptr,nullptr);
    if(!m_Window)
        Core::Log(ErrorType::Error,"Failed to create window");
        glfwMakeContextCurrent(m_Window);
        glfwSetCursorPos(m_Window, 300, 200);

      
        


 }