#pragma once
#include "Application.h"
#include "Utils.h"
struct UnitEX{
    std::string Name{};
    Float2 Position{};
    int Health{};
};
class SerializationTestingLayer :public Layer {
public:
    SerializationTestingLayer();
    void OnCreate()override;
    void OnUpdate(double deltatime)override;
    void OnGUI()override;
    void OnDestroy()override;
    void OnEvent(Event& event)override;
	void OnRender(double deltime)override;


    //Get functions

private:
    
    void OnKeyBoardEvent(KeyBoardEvent& event);
    void OnWindowShutDownEvent(WindowShutDownEvent& event);
    void OnMouseEvent(MouseEvent& event);



    std::vector<UnitEX> m_Units{};
    Serializer m_Serializer{};
    //Controlls
    float m_Move{ -1.0 };
    float m_Direct{ 0.005f };
};  
