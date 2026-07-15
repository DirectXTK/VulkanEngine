#include "RendererFrontEnd.h"
Render::Render(Renderer* renderer): m_Renderer(renderer), m_CurrentQueue(m_RenderQueues[m_CurrentFrame]){
}
void Render::FinishQueue(){
    if(m_ReadyFrames == MAX_FRAME_DRAWS){
       return;
    }
    else{
    m_ReadyFrames++;    
    }

    m_CurrentFrame++;
    if(m_CurrentFrame == MAX_FRAME_DRAWS){
        m_CurrentFrame =0;
    }
  
    m_CurrentQueue = m_RenderQueues[m_CurrentFrame];
}
void Render::DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id){
    m_CurrentQueue.push_back({pos,color,size,id});
}
void Render::FinishRender(){
    for(uint64_t i=0 ;i < m_RenderQueues[m_RenderedFrameIndex].size();i++){
        Core::Log(i);
        RenderQueue& data=m_RenderQueues[m_RenderedFrameIndex].at(i);
        m_Renderer->DrawQuad(data.pos,data.color,data.size,data.id.ID);
    }

    m_RenderQueues[m_RenderedFrameIndex].resize(0);
    m_RenderedFrameIndex++;
    if(m_RenderedFrameIndex == MAX_FRAME_DRAWS){
        m_RenderedFrameIndex =0;
    }
    m_ReadyFrames--;
}