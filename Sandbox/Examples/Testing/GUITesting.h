#pragma once
#include "Application.h"
class GUITestingLayer :public Layer {
public:
    GUITestingLayer();
    void OnCreate()override;
    void OnUpdate(double deltatime)override;
    void OnGUI()override;
    void OnDestroy()override;
	void OnRender(double deltime)override;
    void OnEvent(Event& event)override;

    //Get functions

private:
    void OnKeyBoardEvent(KeyBoardEvent& event);
    void OnMouseEvent(MouseEvent& event);

    bool m_SpawnParticles{false};
    Animator* m_Anim{};
    //Controlls
    float m_Move{ -1.0 };
    float m_Direct{ 0.005f };
};