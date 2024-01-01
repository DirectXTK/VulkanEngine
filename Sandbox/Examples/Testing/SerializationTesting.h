#pragma once
#include "Application.h"
#include "Utils.h"
class Unit;
class SerializationTestingLayer :public Layer {
public:
    SerializationTestingLayer();
    void OnCreate()override;
    void OnUpdate(double deltatime)override;
    void OnGUI()override;
    void OnDestroy()override;


    //Get functions

private:
    void Save();
    void Load();


    std::vector<Unit> m_Units{};
    //Controlls
    float m_Move{ -1.0 };
    float m_Direct{ 0.005f };
};
void MakeUnitRandom(Unit* unit);