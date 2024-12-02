#pragma once
#include "Application.h"
#include "Utils.h"
struct Unit;
class SerializationTestingLayer :public Layer {
public:
    SerializationTestingLayer();
    void OnCreate()override;
    void OnUpdate(float deltatime)override;
    void OnGUI()override;
    void OnDestroy()override;


    //Get functions

private:
    void AllocSpaceAndFillData(void** DataPtr,Format format,uint32_t Index);
    void MakeUnitRandom(Unit* unit);

    void Save();
    void Load();


    std::vector<Unit> m_Units{};
    //Controlls
    float m_Move{ -1.0 };
    float m_Direct{ 0.005f };
};
