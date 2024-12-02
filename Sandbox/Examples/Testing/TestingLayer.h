#pragma once
#include "Application.h"
class TestingLayer :public Layer {
public:
    TestingLayer();
    void OnCreate()override;
    void OnUpdate(float deltatime)override;
    void OnGUI()override;
    void OnDestroy()override;


    //Get functions

private:



    //Controlls
    float m_Move{ -1.0 };
    float m_Direct{ 0.005f };
};