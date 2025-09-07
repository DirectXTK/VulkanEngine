#include "DefaultCameraControlls.h"
void DefaultCameraControlls(Application* app, Camera2D* camera) {
    InputSystem* inputsystem =&app->m_InputSystem;
    static float m_Move{ -1.0 };
    static float m_Direct{ 0.005f };
    static float m_Zoom{ 1.0f };
    static float m_Magnification{ 0.015f };
    if (inputsystem->IsKeyPressed(KeyCodes::D)) {
        Float2 pos = camera->GetPosition();
        pos.x += m_Direct*app->GetDeltaTime();
        camera->SetPosition(pos);
    }
     if (inputsystem->IsKeyPressed(KeyCodes::A)) {
        Float2 pos = camera->GetPosition();
        pos.x -= m_Direct*app->GetDeltaTime();
        camera->SetPosition(pos);
    }
     if (inputsystem->IsKeyPressed(KeyCodes::W)) {
        Float2 pos = camera->GetPosition();
        pos.y += m_Direct*app->GetDeltaTime();
        camera->SetPosition(pos);
    }
     if (inputsystem->IsKeyPressed(KeyCodes::S)) {
        Float2 pos = camera->GetPosition();
        pos.y -= m_Direct*app->GetDeltaTime();
        camera->SetPosition(pos);
    }

    if (inputsystem->GetScroll() != 0) {
        Float2 size = camera->GetScale();

        m_Zoom += (inputsystem->GetScroll() * m_Zoom * m_Magnification*app->GetDeltaTime());
        // m_Magnification = m_Magnification * m_Zoom;
         //size.x += m_Zoom;
        // size.y += m_Zoom;

        camera->SetScale({ m_Zoom,m_Zoom });
    }
}