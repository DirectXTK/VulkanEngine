#include "FrameBuffer.h"

    bool FrameBuffer::Init(VkDevice device,SwapChain* swapchain,VkRenderPass renderpass,Image* attachment,uint32_t Attachmentcount){

                    VkImageView* views{};
                    views = new VkImageView[Attachmentcount];
                    for(int i=0;i < Attachmentcount;i++){
                        views[i] = attachment[i].GetImageView();
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


            return true;
    }
