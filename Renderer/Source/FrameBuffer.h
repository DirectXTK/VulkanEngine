#pragma once
#include "RendCore.h"
#include "SwapChain.h"
class FrameBuffer{
    public:
    bool Init(VkDevice device,SwapChain* swapchain,VkRenderPass renderpass,Image* image,uint32_t AttachmentCount);

    VkFramebuffer GetFrameBuffer(uint32_t Index){return m_FrameBuffers;}

    private:
        VkFramebuffer m_FrameBuffers{};
};