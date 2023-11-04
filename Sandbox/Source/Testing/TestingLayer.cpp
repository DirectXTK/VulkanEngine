#include "TestingLayer.h"
TestingLayer::TestingLayer() : Layer("TestingLayer")
{
}
void TestingLayer::OnCreate() {

}

void TestingLayer::OnUpdate(double deltatime)
{
    Camera2D* camera = &m_App->m_Camera;
    InputSystem* inputsystem = &m_App->m_InputSystem;
    Renderer* renderer = m_App->m_Renderer;

    // for(int i =0;i < 26;i++){

    // }
              //m_Renderer->DrawQuad({0.42f,0.0f,0.0f},{0.0f,1.0f,1.0f,0.0f},{0.2f,0.2f});

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
        size.x -= 0.1f * inputsystem->GetScroll();
        size.y -= 0.1f * inputsystem->GetScroll();

        camera->SetScale(size);
    }

    if (inputsystem->IsMouseClicked(MouseCodes::LEFT)) {
        Buffer* buffer = renderer->GetCustomBuffer(0);
        //  Core::Log(ErrorType::Info,"Pos", inputsystem->GetMousePos().x, " ", inputsystem->GetMousePos().y);

        Float2 a = buffer->ReadPixel((uint32_t)inputsystem->GetMousePos().x, (uint32_t)inputsystem->GetMousePos().y, renderer->GetViewPortExtent().width, renderer->GetViewPortExtent().height);
        uint64_t* ID = (uint64_t*)&a;
       // Core::Log(ErrorType::Info, "ID ", *ID);
    }




    // Core::Log( ErrorType::Info,inputsystem->GetScroll());

    // m_Renderer->Statistics();


}

void TestingLayer::OnGUI()
{
}

void TestingLayer::OnDestroy()
{
}



