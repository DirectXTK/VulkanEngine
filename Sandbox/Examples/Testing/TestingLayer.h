#pragma once
#include "Application.h"
class TestingLayer :public Layer {
public:
    TestingLayer();
    void OnCreate()override;
    void OnUpdate(double deltatime)override;
    void OnGUI()override;
    void OnDestroy()override;
	void OnRender(double deltime)override;
    void OnEvent(Event& event)override;

    //Get functions

private:
    void OnKeyBoardEvent(KeyBoardEvent& event);
};

class MenuLayer: public Layer{
    public:
        MenuLayer();
        void OnCreate()override{}
        void OnUpdate(double deltatime)override{}
        void OnGUI()override{}
        void OnDestroy()override{}
	    void OnRender(double deltime)override;
        void OnEvent(Event& event)override;


    private:
        void OnKeyBoardEvent(KeyBoardEvent& event);
    
};