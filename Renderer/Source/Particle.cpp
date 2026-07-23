#include "Particle.h"
#include "Application.h"
void ParticleSystem::UpdateAndDraw(double deltaTime,bool draw){
    Render* renderer = Application::GetRender();
    for(uint32_t i=0;i < MAXPARTICLECOUNT;i++){
        if(m_ParticleProps[i].Alive){
            m_ParticleProps[i].LifeTime -=deltaTime;
            Core::Log("Limetime",   m_ParticleProps[i].LifeTime );
            Core::Log("deltaTime",   deltaTime );
            if(m_ParticleProps[i].LifeTime <= 0.0f){
                if(m_ParticleProps[i].CustomData){
                    if(m_ParticleProps[i].CustomDataDestructor)
                        m_ParticleProps[i].CustomDataDestructor(m_ParticleProps->CustomData);
                    else
                        free(m_ParticleProps[i].CustomData);
                }
                
                m_ParticleProps[i].Alive = false;
            }else{
                //Draw and update velocity
                ParticleProps& prop = m_ParticleProps[i];
                if(!draw)
                    continue;
                if(prop.CustomFunction)
                    prop.CustomFunction(prop);
                if(prop.TextureID !=0)
                    renderer->DrawInstance(prop.Pos,prop.Color,prop.Size,prop.ID,prop.TextureID,prop.TextureIndex);
                else if(prop.Animation){
                    renderer->DrawInstance(prop.Pos,prop.Color,prop.Size,prop.ID,prop.Animation);
                    prop.Animation.Update(deltaTime);
                }else{
                    renderer->DrawInstance(prop.Pos,prop.Color,prop.Size,prop.ID,0,-1);
                }
            }
        }
    }
}
void ParticleSystem::Shutdown(){
    for(uint32_t i=0; i < MAXPARTICLECOUNT;i++){
        m_ParticleProps[i] = ParticleProps();
    }
    memset(m_ParticleProps,0,sizeof(ParticleProps)*MAXPARTICLECOUNT);
}

void ParticleSystem::DrawParticle(const ParticleProps& props){

    #ifdef DEBUG
    if(props.CustomData && !props.CustomDataDestructor)
        Core::Log(ErrorType::Warning,"Memory leak you need to create a custom data destructor ParticleProps.CustomDataDesctructor is nullptr now using free() function");
    #endif

    m_ParticleProps[m_CurrentParticle] = props;
    m_ParticleProps[m_CurrentParticle].Alive = true;
    if(m_CurrentParticle+1 >= MAXPARTICLECOUNT)
        m_CurrentParticle = 0;
    else
        m_CurrentParticle++;
}