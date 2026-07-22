#include "MultiThreadingLayer.h"
    MultiThreadingLayer::MultiThreadingLayer():Layer("MultiThreadingLayer"){

    }
    void MultiThreadingLayer::OnCreate(){

    }
    void MultiThreadingLayer::OnUpdate(double deltaTime){
    }
    void MultiThreadingLayer::OnRender(double delaTime){
        Render* render= Application::GetRender();
        for(uint32_t i=0 ;i < 1;i++){

            render->DrawQuad({Core::RandomFloat(-1.0f,1.0f),0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{0.2f,0.2f},0);
        }
    }
    void MultiThreadingLayer::OnDestroy(){


    }
    void MultiThreadingLayer::OnGUI(){

    }




