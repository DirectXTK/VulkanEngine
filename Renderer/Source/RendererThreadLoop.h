#pragma once
#include "RendCore.h"
#include "Renderer.h"
#include "GUI.h"
void RendererLoop(Renderer* renderer,GUIRenderer* gui,Render* render){
    if(!renderer){
        Core::Log(ErrorType::FatalError,"Renderer not initialized is nullptr (RendererLoop())");
    }
    double timePassed{};
    double updateInterval{100.f};
    while(!renderer->IsShuttingDown()){
        if(render->GetReadyFrameCount() >0){
            double start = Time::GetTimeNs();
            renderer->BeginFrame(render->GetCameraPos(),render->GetCameraSize(),render->GetViewProj());
            render->FinishRenderGeometry();
            gui->BeginGUI();
            renderer->BeginGUIFrame();
            
            render->FinishRenderGUI();
            
            
            
            gui->EndGUI();
            render->EndFrame();
            renderer->EndFrame();
            renderer->RunRendererChangeQueue();

            double deltaTime = (Time::GetTimeNs()-start)/1000000.f;
            timePassed+=deltaTime;
            if(timePassed >= updateInterval){
                renderer->SetRendererThreadFrameTime((float)deltaTime);
                timePassed = 0;
            }
        }       
    }
    Core::Log("ShuttingDown RendererThread");

}