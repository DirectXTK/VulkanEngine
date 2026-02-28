#pragma once
#include "AppCore.h"
#define MAXPARTICLECOUNT 5000
struct InstanceParticleData{
    Float3 Position{};
};
struct VertexParticleData{
    Float3 BasePosition{};
    Float4 Color{};
    Float2 TextureCords{};
    uint32_t TextureID{};
};
struct ParticleProps{
    Float2 Pos{};
    Float4 Color{};
    GUUID TextureID{0};  
    Float2 Size{};

    Float2 Velocity{};
    float LifeTime{};
    bool Alive{false};
};
class ParticleSystem{
public:

    void DrawParticle(const ParticleProps& props);
    void UpdateAndDraw(double deltaTime);

private:
    ParticleProps m_ParticleProps[MAXPARTICLECOUNT];
    uint32_t m_CurrentParticle{};
};