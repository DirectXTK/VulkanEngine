#include "Particle.h"
#include "Application.h"
void ParticleSystem::UpdateAndDraw(double deltaTime){
    Renderer* renderer = Application::GetRenderer();
    for(uint32_t i=0;i < MAXPARTICLECOUNT;i++){
        if(m_ParticleProps[i].Alive){
            m_ParticleProps[i].LifeTime -=deltaTime;
            if(m_ParticleProps[i].LifeTime <= 0.0f){
                m_ParticleProps[i].Alive = false;
            }else{
                //Draw and update velocity
                ParticleProps& prop = m_ParticleProps[i];
                prop.Pos.x += prop.Velocity.x;
                prop.Pos.y += prop.Velocity.y;
            
                renderer->DrawParticle(prop.Pos,prop.Color,prop.Size,prop.TextureID);
            }
        }
    }
}
void ParticleSystem::DrawParticle(const ParticleProps& props){
    m_ParticleProps[m_CurrentParticle] = props;
    m_ParticleProps[m_CurrentParticle].Alive = true;
    if(m_CurrentParticle+1 >= MAXPARTICLECOUNT)
        m_CurrentParticle = 0;
    else
        m_CurrentParticle++;
}