#pragma once
#include "RendCore.h"
#include "SwapChain.h"
class FrameBuffer{
    public:
    bool Init(VkDevice device,SwapChain* swapchain,VkRenderPass renderpass,Texture** texture,uint32_t AttachmentCount);

    VkFramebuffer GetFrameBuffer(uint32_t Index){return m_FrameBuffers;}

    ~FrameBuffer();
    private:
        VkDevice m_Device{};
        VkFramebuffer m_FrameBuffers{};
};