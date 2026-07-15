#pragma once
#include "AppCore.h"
#include "Renderer.h"
class Render{
    public:

    Render(Renderer* renderer);
    void StartQueue(Camera2D& camera){m_ViewProj[m_CurrentFrame] = camera.GetViewProj();}
    void DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id);
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
        
    };
    Renderer* m_Renderer{};

    uint32_t m_RenderedFrameIndex{};
    uint32_t m_CurrentFrame{};
    uint32_t m_ReadyFrames{0};
    std::vector<RenderQueue>& m_CurrentQueue;

    glm::mat4 m_ViewProj[MAX_FRAME_DRAWS];
    Float2 m_CameraSize[MAX_FRAME_DRAWS];
    Float2 m_CameraPos[MAX_FRAME_DRAWS];
    std::vector<RenderQueue> m_RenderQueues[MAX_FRAME_DRAWS];
};