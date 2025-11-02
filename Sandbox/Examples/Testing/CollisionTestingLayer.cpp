#include "CollisionTestingLayer.h"
#include "DefaultCameraControlls.h"
CollisionTestingLayer::CollisionTestingLayer(): Layer("CollisionTestingLayer")
{
}

void CollisionTestingLayer::OnCreate()
{
	//Application::LoadAssets("C:\\Repos\\VulkanEngine\\Resources\\Animation\\", ResourceType::ANIMATION);
	//Application::LoadAssets("C:\\Repos\\VulkanEngine\\Resources\\Textures\\", ResourceType::TEXTURE);

	Application::GetCurrentCamera()->SetPosition({ 0.0f,0.0f });


	m_Colliders.push_back({ {0.0f,0.0f} });
	m_Colliders.push_back({ {0.7f,0.0f} });

	m_Colliders[0].collider = Application::CreateCollider(&m_Colliders[0].Position,&m_Size);
	m_Colliders[1].collider = Application::CreateCollider(&m_Colliders[1].Position, &m_Size);

	m_Colors = new Float4[m_ChunkHeight * m_ChunkWidth];
	for (uint32_t i = 0; i < m_ChunkHeight * m_ChunkWidth; i++)
		m_Colors[i] = { Core::RandomFloat(0.0f,1.0f),Core::RandomFloat(0.0f,1.0f) ,Core::RandomFloat(0.0f,1.0f) ,1.0f };

}
void CollisionTestingLayer::OnRender(double deltime){

}
void CollisionTestingLayer::OnUpdate(float Deltatime)
{
	ChunkSystem system(2,2);
	Renderer* renderer= Application::GetRenderer();

	for (uint32_t i = 0; i < 4; i++) {
		Chunk* chunk = system.GetChunk(i);
		
		for (uint32_t y = 0; y < chunk->Size; y++) {
			for (uint32_t x = 0; x < chunk->Size; x++) {
				renderer->DrawQuad({ x*0.02f+chunk->ChunkOffset.x,y * 0.02f+ chunk->ChunkOffset.y}, m_Colors[i], {0.02f ,0.02f}, 0);

			}
		}
	}
	DefaultCameraControlls(Application::GetCurrentCamera());
}

void CollisionTestingLayer::OnDestroy()
{
}

void CollisionTestingLayer::OnGUI()
{
}
