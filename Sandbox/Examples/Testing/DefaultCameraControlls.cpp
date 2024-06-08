#include "DefaultCameraControlls.h"
#pragma once
#include "Application.h"
void DefaultCameraControlls(InputSystem* inputsystem, Camera2D* camera) {
    static float m_Move{ -1.0 };
    static float m_Direct{ 0.005f };
    static float m_Zoom{ 1.0f };
    static float m_Magnification{ 0.2f };
    if (inputsystem->IsKeyPressed(KeyCodes::D)) {
        Float2 pos = camera->GetPosition();
        pos.x += m_Direct;
        camera->SetPosition(pos);
    }
    else if (inputsystem->IsKeyPressed(KeyCodes::A)) {
        Float2 pos = camera->GetPosition();
        pos.x -= m_Direct;
        camera->SetPosition(pos);
    }
    else if (inputsystem->IsKeyPressed(KeyCodes::W)) {
        Float2 pos = camera->GetPosition();
        pos.y += m_Direct;
        camera->SetPosition(pos);
    }
    else if (inputsystem->IsKeyPressed(KeyCodes::S)) {
        Float2 pos = camera->GetPosition();
        pos.y -= m_Direct;
        camera->SetPosition(pos);
    }

    if (inputsystem->IsKeyPressed(KeyCodes::X)) {
        Float2 size = camera->GetScale();
        size.x += 0.1f;
        size.y += 0.1f;

        camera->SetScale(size);
    }
    if (inputsystem->IsKeyPressed(KeyCodes::Z)) {
        Float2 size = camera->GetScale();
        size.x -= 0.1f;
        size.y -= 0.1f;

        camera->SetScale(size);
    }
    if (inputsystem->GetScroll() != 0) {
        Float2 size = camera->GetScale();

        m_Zoom += (inputsystem->GetScroll() * m_Zoom * m_Magnification);
        // m_Magnification = m_Magnification * m_Zoom;
         //size.x += m_Zoom;
        // size.y += m_Zoom;

        camera->SetScale({ m_Zoom,m_Zoom });
    }
}