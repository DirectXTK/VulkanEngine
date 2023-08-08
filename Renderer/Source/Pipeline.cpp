#include "Pipeline.h"

VkRenderPass CreateRenderPass(VkDevice device,VkFormat format){

    VkRenderPass RenderPass{};

    
    VkAttachmentDescription ColorAttach2{};
    ColorAttach2.format = VK_FORMAT_R32G32_UINT;
    ColorAttach2.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttach2.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttach2.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttach2.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttach2.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    ColorAttach2.initialLayout =VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttach2.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;


    VkAttachmentReference ColorAttachRef2{};
    ColorAttachRef2.attachment = 1;
    ColorAttachRef2.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    VkAttachmentDescription ColorAttach{};
    ColorAttach.format = format;
    ColorAttach.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    ColorAttach.initialLayout =VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference ColorAttachRef{};
    ColorAttachRef.attachment = 0;
    ColorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference refs[2]={ColorAttachRef,ColorAttachRef2};

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint= VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 2;
    subpass.pColorAttachments = refs;

    std::array<VkSubpassDependency,2> subpassDependency{};
    
    subpassDependency[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency[0].dstSubpass = 0;
    subpassDependency[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    subpassDependency[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    subpassDependency[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    subpassDependency[1].srcSubpass = 0;
    subpassDependency[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    subpassDependency[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    subpassDependency[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkAttachmentDescription attachdescs[2] = {ColorAttach,ColorAttach2};

    VkRenderPassCreateInfo RenderPassinfo{};
    RenderPassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassinfo.attachmentCount = 2;
    RenderPassinfo.pAttachments= attachdescs;
    RenderPassinfo.subpassCount = 1;
    RenderPassinfo.pSubpasses = &subpass;
    RenderPassinfo.dependencyCount = subpassDependency.size();
    RenderPassinfo.pDependencies = subpassDependency.data();


    VkResult result = vkCreateRenderPass(device,&RenderPassinfo,nullptr,&RenderPass);
    if(result != VK_SUCCESS)
        Core::Log(ErrorType::Error,"Failed to create renderpass.");
    return RenderPass;
}
