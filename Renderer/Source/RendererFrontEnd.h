#pragma once
#include "AppCore.h"
#include "Renderer.h"
class Render{
    public:

    Render(Renderer* renderer);
    void StartQueue(Camera2D& camera){m_ViewProj[m_CurrentFrame] = camera.GetViewProj();}

    void DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id,GUUID textureID,int32_t textureIndex);
    void DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id);
    
    void DrawText(const char* Message,uint64_t bufferSize, Float2 Position, Float2 BoundingBox[4], float FixedPadding,float CharSizePixels,GUUID id,int64_t PointerIndex); 


    void FinishQueue();
    void FinishRender();

    glm::mat4 GetViewProj(){return m_ViewProj[m_RenderedFrameIndex];}
    uint32_t GetRenderedFrame(){return m_RenderedFrameIndex;}
    uint32_t GetReadyFrameCount(){return m_ReadyFrames;}

    Float2 GetCameraSize(){return m_CameraSize[m_RenderedFrameIndex];}
    Float2 GetCameraPos(){return m_CameraPos[m_RenderedFrameIndex];}

    private:
    struct RenderQueue{
        Float3 pos{};
        Float4 color{};
        Float2 size{};
        GUUID id{};
        GUUID textureID{0};
        int32_t textureId{-1};
        
    };
    struct RenderQueueText{
        char* message{};
        uint64_t bufferSize{};  
        Float2 pos{};
        //Size 4 array
        Float2* boundingBox;
        float fixedPadding{};
        float charPixelSize{};
        GUUID id{};
        int64_t pointerIndex{};

        //~RenderQueueText(){
         //   delete[] message;
        //}
    };
    Renderer* m_Renderer{};

    uint32_t m_RenderedFrameIndex{};
    uint32_t m_CurrentFrame{};
    std::atomic<uint32_t> m_ReadyFrames{0};

    glm::mat4 m_ViewProj[MAX_FRAME_DRAWS];
    Float2 m_CameraSize[MAX_FRAME_DRAWS];
    Float2 m_CameraPos[MAX_FRAME_DRAWS];
    std::vector<RenderQueue> m_RenderQueues[MAX_FRAME_DRAWS];
    std::vector<RenderQueueText> m_RenderQueuesText[MAX_FRAME_DRAWS];
};