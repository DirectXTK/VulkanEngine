#include "RendererFrontEnd.h"
Render::Render(Renderer* renderer): m_Renderer(renderer){
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
  
}
void Render::DrawText(const char* Message,uint64_t bufferSize, Float2 Position, Float2 BoundingBox[4], float FixedPadding,float CharSizePixels,GUUID id,int64_t PointerIndex){
    char* msg = new char[strlen(Message)*sizeof(char)];
    strcpy(msg,Message);
    m_RenderQueuesText[m_CurrentFrame].push_back({msg,bufferSize,Position,BoundingBox,FixedPadding,CharSizePixels,id,PointerIndex,});
}
void Render::DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id){
    m_RenderQueues[m_CurrentFrame].push_back({pos,color,size,id,0,-1});
}
void Render::DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id,GUUID textureID,int textureIndex){
        m_RenderQueues[m_CurrentFrame].push_back({pos,color,size,id,textureID,textureIndex});
}
void Render::FinishRender(){
    //somehow seperate the GUI objects from normal ones.
    for(uint64_t i=0 ;i < m_RenderQueues[m_RenderedFrameIndex].size();i++){
        RenderQueue& data=m_RenderQueues[m_RenderedFrameIndex].at(i);
        if(data.textureID ==0)
            m_Renderer->DrawQuad(data.pos,data.color,data.size,data.id.ID);
        else
            m_Renderer->DrawQuad(data.pos,data.color,data.size,data.textureID,data.id.ID,data.textureId);
    }
    for(uint64_t i=0 ;i < m_RenderQueuesText[m_RenderedFrameIndex].size();i++){
        RenderQueueText& data=m_RenderQueuesText[m_RenderedFrameIndex].at(i);

        m_Renderer->RenderText(data.message,data.bufferSize,data.pos,data.boundingBox,data.fixedPadding,data.charPixelSize,data.id,data.pointerIndex);
    }

    m_RenderQueues[m_RenderedFrameIndex].resize(0);
    m_RenderQueuesText[m_RenderedFrameIndex].resize(0);
    m_RenderedFrameIndex++;
    if(m_RenderedFrameIndex == MAX_FRAME_DRAWS){
        m_RenderedFrameIndex =0;
    }   
    m_ReadyFrames--;
}