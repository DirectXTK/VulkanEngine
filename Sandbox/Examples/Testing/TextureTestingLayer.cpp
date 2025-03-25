#include "TextureTestingLayer.h"

TextureTestingLayer::TextureTestingLayer(): Layer("TextureTestingLayer")
{
}

void TextureTestingLayer::OnCreate()
{
    m_App->LoadAssets("C:\\Repos\\VulkanEngine\\Resources\\Textures\\", AssetType::TEXTUREATLAS);
    m_App->LoadAssets("C:\\Repos\\VulkanEngine\\Resources\\Textures\\", AssetType::TEXTURE);

	//m_Units.push_back({ Core::GetStringHash("Examples\\BLOOD_KNIGHT.png"),{0.0f,0.0f} });
    m_Units.push_back({ Core::GetStringHash("Texture1.png"),{0.1f,0.0f} });
    m_Units.push_back({ Core::GetStringHash("Texture2.png"),{0.2f,0.0f} });
    m_Units.push_back({ Core::GetStringHash("Texture3.png"),{0.3f,0.0f} });
    m_Units.push_back({ Core::GetStringHash("Texture4.png"),{0.4f,0.0f} });


    m_Units.push_back({ Core::GetStringHash("Examples\\Test2.json"),  { 0.5f,0.0f },0 });
    Camera2D* camera = &m_App->m_Camera;
    camera->SetScale({m_Zoom,m_Zoom});

    Core::Log(ErrorType::Info, "TextureAtlasCount: ", m_App->m_AssetManager.GetAssetCount(AssetType::TEXTUREATLAS));
    Core::Log(ErrorType::Info, "TextureCount: ", m_App->m_AssetManager.GetAssetCount(AssetType::TEXTURE));

}

void TextureTestingLayer::OnUpdate(float DeltaTime)
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

        m_Zoom += (inputsystem->GetScroll()*m_Zoom*m_Magnification);
       // m_Magnification = m_Magnification * m_Zoom;
        //size.x += m_Zoom;
       // size.y += m_Zoom;

        camera->SetScale({m_Zoom,m_Zoom});
    }
    


	for (uint32_t i = 0; i < m_Units.size(); i++) {
		m_App->m_Renderer->DrawQuad(m_Units[i].Position, { 1.0f,1.0f,1.0f,1.0f }, { 0.04f,0.04f },m_Units[i].TextureHandle, 0,m_Units[i].TextureIndex);
        

	}
    
}

void TextureTestingLayer::OnGUI()
{
    /*
	if (m_App->m_GUIRenderer->Button("Create Blood knight", {0.0f,-0.9f}, {1.0f,1.0f,1.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT, Core::GetStringHash("GUI\\SpawnButton"), false)) {
        m_Units.push_back({ Core::GetStringHash("Examples\\BLOOD_KNIGHT"),{Core::RandomFloat(-1.0f,1.0f),Core::RandomFloat(-1.0f,1.0f)}});

	}
    if (m_App->m_GUIRenderer->Button("ChangeTextureID", {0.2f,-0.9f}, {1.0f,1.0f,1.0f,1.0f}, {0.1f,0.1f}, MouseCodes::LEFT, Core::GetStringHash("GUI\\ChangeTextureButton"), false)) {
        m_Units[3].TextureIndex++;

    }
    */
}

void TextureTestingLayer::OnDestroy()
{
}
