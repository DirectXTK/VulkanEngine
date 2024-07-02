#include "Pipeline.h"
#include "Shader.h"
VkRenderPass Pipeline::CreateRenderPass(VkDevice device, VkFormat format) {

    VkRenderPass RenderPass{};

    //Depth stencil
    VkAttachmentDescription DepthStencilAttachment{};
    DepthStencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthStencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthStencilAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthStencilAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    DepthStencilAttachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
    DepthStencilAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    DepthStencilAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthStencilAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    VkAttachmentReference DepthStencilAttachmentRef{};
    DepthStencilAttachmentRef.attachment = 2;
    DepthStencilAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription ColorAttach2{};
    ColorAttach2.format = VK_FORMAT_R32G32_UINT;
    ColorAttach2.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttach2.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttach2.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttach2.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttach2.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    ColorAttach2.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttach2.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;


    VkAttachmentReference ColorAttachRef2{};
    ColorAttachRef2.attachment = 1;
    ColorAttachRef2.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    VkAttachmentDescription ColorAttach{};
    ColorAttach.format = format;
    ColorAttach.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

    ColorAttach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference ColorAttachRef{};
    ColorAttachRef.attachment = 0;
    ColorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference refs[3] = { ColorAttachRef,ColorAttachRef2 ,DepthStencilAttachmentRef };




    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 2;
    subpass.pColorAttachments = refs;
    subpass.pDepthStencilAttachment = &DepthStencilAttachmentRef;

    std::array<VkSubpassDependency, 2> subpassDependency{};

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

    VkAttachmentDescription attachdescs[3] = { ColorAttach,ColorAttach2,DepthStencilAttachment };

    VkRenderPassCreateInfo RenderPassinfo{};
    RenderPassinfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassinfo.attachmentCount = 3;
    RenderPassinfo.pAttachments = attachdescs;
    RenderPassinfo.subpassCount = 1;
    RenderPassinfo.pSubpasses = &subpass;
    RenderPassinfo.dependencyCount = (uint32_t)subpassDependency.size();
    RenderPassinfo.pDependencies = subpassDependency.data();

    VkResult result = vkCreateRenderPass(device, &RenderPassinfo, nullptr, &RenderPass);
    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to create renderpass.");
    return RenderPass;
}


VkPipelineLayout Pipeline::CreatePipelineLayout(VkDevice device,VkDescriptorSetLayout* DescriptorSetLayout,uint32_t DescriptorSetCount)
{
    VkPipelineLayout PipelineLayout{};
    VkPipelineLayoutCreateInfo pipelinelayout{};
    pipelinelayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelinelayout.setLayoutCount = DescriptorSetCount;
    pipelinelayout.pSetLayouts = DescriptorSetLayout;
    pipelinelayout.pushConstantRangeCount = 0;

    VkResult result = vkCreatePipelineLayout(device, &pipelinelayout, nullptr, &PipelineLayout);
    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to create pipeline layout.");
    return PipelineLayout;
}

VkPipeline Pipeline::CreatePipeline(PipelineDesc& desc,VkDevice device)
{
    VkPipeline outputPipeline{};
    VkShaderModule Vertex= ShaderDesc::CreateShader({ ShaderType::VertexShader,"C:/Repos/VulkanEngine/Shaders/Vertex.spv" },device);
    VkShaderModule Pixel= ShaderDesc::CreateShader({ ShaderType::PixelShader,"C:/Repos/VulkanEngine/Shaders/Fragment.spv" },device);


    VkPipelineShaderStageCreateInfo vertexstage{};
    vertexstage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexstage.module = Vertex;
    vertexstage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexstage.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentstage{};
    fragmentstage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentstage.module = Pixel;
    fragmentstage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentstage.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = { vertexstage,fragmentstage };

    VkVertexInputBindingDescription vertexinputdesc{};
    vertexinputdesc.stride = desc.VertexInputStride;
    vertexinputdesc.binding = 0;
    vertexinputdesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


    VkVertexInputAttributeDescription* vertexinputattr = new VkVertexInputAttributeDescription[desc.VertexStageInputCount];
    
    for (int i = 0; i < desc.VertexStageInputCount; i++) {
        vertexinputattr[i].binding = desc.VertexStageInput[i].Binding;
        vertexinputattr[i].location = desc.VertexStageInput[i].Location;
        vertexinputattr[i].format = desc.VertexStageInput[i].Format;
        vertexinputattr[i].offset = desc.VertexStageInput[i].Offset;
    }





    VkPipelineVertexInputStateCreateInfo vertexinputstate{};
    vertexinputstate.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexinputstate.vertexAttributeDescriptionCount = desc.VertexStageInputCount;
    vertexinputstate.vertexBindingDescriptionCount = 1;
    vertexinputstate.pVertexBindingDescriptions = &vertexinputdesc;
    vertexinputstate.pVertexAttributeDescriptions = vertexinputattr;


    VkPipelineInputAssemblyStateCreateInfo inputassassembly{};
    inputassassembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputassassembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputassassembly.primitiveRestartEnable = false;

    //Depth stencil
    VkStencilOpState FrontState{};
    FrontState.compareOp = VK_COMPARE_OP_ALWAYS;
    FrontState.failOp = VK_STENCIL_OP_REPLACE;
    FrontState.passOp = VK_STENCIL_OP_REPLACE;
    FrontState.writeMask = 0x00;
    FrontState.compareMask = 0x00;

    VkStencilOpState BackState{};
    BackState.writeMask = 0xFF;
    BackState.compareOp = VK_COMPARE_OP_ALWAYS;
    BackState.failOp = VK_STENCIL_OP_REPLACE;
    BackState.passOp = VK_STENCIL_OP_REPLACE;
    BackState.compareMask = 0x00;

    VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo{};
   DepthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
   DepthStencilCreateInfo.depthBoundsTestEnable = false;
   DepthStencilCreateInfo.stencilTestEnable = true;
   //DepthStencilCreateInfo.front = FrontState;
  // DepthStencilCreateInfo.back = BackState;



    //
    VkRect2D Scissor{};
    Scissor.offset = { 0,0 };
    Scissor.extent = {(uint32_t)desc.Viewport.width,(uint32_t)desc.Viewport.height};

    VkPipelineViewportStateCreateInfo viewportinfo{};
    viewportinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportinfo.viewportCount = 1;
    viewportinfo.pViewports = &desc.Viewport;
    viewportinfo.scissorCount = 1;
    viewportinfo.pScissors = &Scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationstate{};
    rasterizationstate.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationstate.depthClampEnable = false;
    rasterizationstate.rasterizerDiscardEnable = false;
    rasterizationstate.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationstate.lineWidth = 1.0f;
    rasterizationstate.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationstate.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationstate.depthBiasEnable = false;

    VkPipelineMultisampleStateCreateInfo multisampleinfo{};
    multisampleinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleinfo.sampleShadingEnable = false;
    multisampleinfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;



    VkPipelineColorBlendAttachmentState blendattachment{};
    blendattachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendattachment.blendEnable = true;

    blendattachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendattachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendattachment.colorBlendOp = VK_BLEND_OP_ADD;
    blendattachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendattachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendattachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendAttachmentState blendattachment2{};
    blendattachment2.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT;
    blendattachment2.blendEnable = false;

    blendattachment2.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendattachment2.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendattachment2.colorBlendOp = VK_BLEND_OP_ADD;
    blendattachment2.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendattachment2.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendattachment2.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendAttachmentState blendattachments[] = { blendattachment,blendattachment2 };

    VkPipelineColorBlendStateCreateInfo blendstate{};
    blendstate.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendstate.logicOpEnable = false;
    blendstate.pAttachments = blendattachments;
    blendstate.attachmentCount = 2;


    VkDynamicState DynamicState[5] = { VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE,VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,VK_DYNAMIC_STATE_STENCIL_OP,VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,VK_DYNAMIC_STATE_STENCIL_REFERENCE };

    VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    DynamicStateCreateInfo.dynamicStateCount = 5;
    DynamicStateCreateInfo.pDynamicStates = DynamicState;
    
 


    VkGraphicsPipelineCreateInfo pipeline{};
    pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.stageCount = 2;
    pipeline.pStages = stages;
    pipeline.pVertexInputState = &vertexinputstate;
    pipeline.pInputAssemblyState = &inputassassembly;
    pipeline.pViewportState = &viewportinfo;
    pipeline.pRasterizationState = &rasterizationstate;
    pipeline.pMultisampleState = &multisampleinfo;
    pipeline.pColorBlendState = &blendstate;
    pipeline.pDepthStencilState = &DepthStencilCreateInfo;
    pipeline.pDynamicState = &DynamicStateCreateInfo;
    pipeline.layout = desc.PipelineLayout;
    pipeline.renderPass = desc.RenderPass;
    pipeline.subpass = 0;

    pipeline.basePipelineIndex = -1;

    VkResult result = vkCreateGraphicsPipelines(device, nullptr, 1, &pipeline, nullptr, &outputPipeline);
    if (result != VK_SUCCESS)
        Core::Log(ErrorType::Error, "Failed to create pipeline.");
    return outputPipeline;
}
