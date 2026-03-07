#pragma once
#include "AppCore.h"
#include "Animator.h"
#define MAXPARTICLECOUNT 10000
struct InstanceParticleData{
    Float3 Position{};
    GUUID ID{0};
};
struct VertexParticleData{
    Float3 BasePosition{};
    Float4 Color{};
    Float2 TextureCords{0.0f,0.0f};
    uint32_t TextureID{};
};
struct ParticleProps{
    Float2 Pos{};
    Float4 Color{};
    GUUID TextureID{0};  
    Float2 Size{};
    GUUID ID{0};
    int TextureIndex{-1};
    Animator Animation{};

    void* CustomData{};
    void(*CustomFunction)(ParticleProps&);
    void(*CustomDataDestructor)(void*);

    Float2 Velocity{};
    float LifeTime{};
    bool Alive{false};
};
class ParticleSystem{
public:

    void DrawParticle(const ParticleProps& props);
    void UpdateAndDraw(double deltaTime);

    void Shutdown();
private:
    ParticleProps m_ParticleProps[MAXPARTICLECOUNT];
    uint32_t m_CurrentParticle{};
};