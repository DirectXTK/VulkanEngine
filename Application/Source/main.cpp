#include "Application.h"
int main(){

 
    ApplicationSpecs specs{};
    specs.WindowTitle = "Vulkan";

    Application app(specs);


   

    app.Run();
    return 1;
}