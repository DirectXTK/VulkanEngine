#include "FrameBuffer.h"

    bool FrameBuffer::Init(VkDevice device,SwapChain* swapchain,VkRenderPass renderpass,Texture** attachment,uint32_t Attachmentcount){
                    m_Device = device;

                    VkImageView* views{};
                    views = new VkImageView[Attachmentcount];
                    for(uint32_t  i=0;i < Attachmentcount;i++){
                        views[i] = (*attachment[i]).GetImageView();
                    }

                    VkFramebufferCreateInfo createinfo{};
                    createinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                    createinfo.renderPass = renderpass;
                    createinfo.attachmentCount = Attachmentcount;
                    createinfo.pAttachments = views;
                    createinfo.width = swapchain->GetExtent().width;
                    createinfo.height = swapchain->GetExtent().height;
                    createinfo.layers = 1;

                    VkResult result = vkCreateFramebuffer(device,&createinfo,nullptr,&m_FrameBuffers);
                    if(result != VK_SUCCESS)
                        Core::Log(ErrorType::Error,"Failed to create framebuffer.");
                    delete[] views;


            return true;
    }
    FrameBuffer::~FrameBuffer(){
        vkDestroyFramebuffer(m_Device,m_FrameBuffers,nullptr);

    }
