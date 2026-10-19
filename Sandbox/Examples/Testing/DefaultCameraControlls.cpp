#include "DefaultCameraControlls.h"
void DefaultCameraControlls(Camera2D* camera) {
    float deltaTime = Application::GetDeltaTime();
    static float m_Move{ -1.0 };
    static float m_Direct{ 0.05f };
    static float m_Zoom{ 1.0f };
    static float m_Magnification{ 0.015f };
    if (Application::IsKeyPressed(KeyCodes::D)) {
        Float2 pos = camera->GetPosition();
        pos.x += m_Direct*deltaTime;
        camera->SetPosition(pos);
    }
     if (Application::IsKeyPressed(KeyCodes::A)) {
        Float2 pos = camera->GetPosition();
        pos.x -= m_Direct*deltaTime;
        camera->SetPosition(pos);
    }
     if (Application::IsKeyPressed(KeyCodes::W)) {
        Float2 pos = camera->GetPosition();
        pos.y += m_Direct*deltaTime;
        camera->SetPosition(pos);
    }
     if (Application::IsKeyPressed(KeyCodes::S)) {
        Float2 pos = camera->GetPosition();
        pos.y -= m_Direct*deltaTime;
        camera->SetPosition(pos);
    }

    if (Application::GetScroll() != 0) {
        Float2 size = camera->GetScale();

        m_Zoom += (Application::GetScroll() * m_Zoom * m_Magnification*deltaTime);
         m_Magnification = m_Magnification * m_Zoom;
         size.x += m_Zoom;
         size.y += m_Zoom;

        camera->SetScale({ m_Zoom,m_Zoom });
    }
}