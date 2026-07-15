#pragma once
#include "RendCore.h"
#include "Renderer.h"
#include "GUI.h"
void RendererLoop(Renderer* renderer,GUIRenderer* gui,Render* render){
    if(!renderer){
        Core::Log(ErrorType::FatalError,"Renderer not initialized is nullptr (RendererLoop())");
    }
    while(!renderer->IsShuttingDown()){
        if(render->GetReadyFrameCount() >0){
            renderer->BeginFrame(render->GetCameraPos(),render->GetCameraSize(),render->GetViewProj(),0);
          //  gui->BeginGUI();
           // renderer->BeginGUIFrame();

        
       

   
             //   gui->EndGUI();
                renderer->EndFrame();
                render->FinishRender();
        }       
        
    }
    Core::Log("ShuttingDown RendererThread");

}