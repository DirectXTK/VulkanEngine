#pragma once
#include "Application.h"

class MultiThreadingLayer : public Layer{
    public:
    MultiThreadingLayer();
    void OnCreate()override;
    void OnUpdate(double deltaTime)override;
    void OnRender(double delaTime)override;
    void OnDestroy()override;
    void OnEvent(Event& event)override;
    void OnGUI()override;


    private:
    void OnMouseEvent(MouseEvent& event);
    void OnKeyboardEvent(KeyBoardEvent& event);

    std::vector<Float2> m_OBJ{};
};