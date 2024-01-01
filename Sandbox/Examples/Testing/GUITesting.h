#pragma once
#include "Application.h"
class GUITestingLayer :public Layer {
public:
    GUITestingLayer();
    void OnCreate()override;
    void OnUpdate(double deltatime)override;
    void OnGUI()override;
    void OnDestroy()override;


    //Get functions

private:



    //Controlls
    float m_Move{ -1.0 };
    float m_Direct{ 0.005f };
};