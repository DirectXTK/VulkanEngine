#include "Renderer.h"
#include "SwapChain.h"
#include "Pipeline.h"

 Renderer::Renderer(RendererDesc desc,GLFWwindow* window){
   m_Window = window;

    //m_VertexBufferSize = 3*100;

    CreateInstance();
    CreateDebugger();
    CreateSurface(window,&m_Surface);
    GetPhysicalDevice();

    CreateLogicalDevice();

     m_SwapChain = new SwapChain(m_Instance,m_PhysicalDevice,m_Device,m_Surface,GetQueueFamilies(m_PhysicalDevice));
    m_SwapChainDetails = m_SwapChain->GetSwapChainCapabilities();
    Core::Log(ErrorType::FatalError,"ADwadadad");

        m_SwapChain->CreateSwapChain();
    Core::Log(ErrorType::FatalError,"ADwadadad");

      
             //Matrices
        m_UniformBuffers = new UniformBuffer[MAX_FRAME_DRAWS];
        for(int i =0;i < MAX_FRAME_DRAWS;i++){
        m_UniformBuffers[i].Init(m_Device,m_PhysicalDevice,sizeof(glm::mat4));
     
        }
          m_DescriptorLayout.Init(m_Device);
        m_DescriptorLayout.WriteTo(m_Device,*m_UniformBuffers[0].GetBuffer(),sizeof(glm::mat4));


        CreatePipeline();

        VkFormat format = Core::ChooseBestFormat(m_PhysicalDevice,{ VK_FORMAT_R32G32_UINT },VK_IMAGE_TILING_OPTIMAL,VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);

         m_ColorAttachments.resize(m_SwapChain->GetSwapChainImageCount());
         for(int i =0;i < m_ColorAttachments.size();i++){

              m_ColorAttachments[i]=Image(m_PhysicalDevice,m_Device,format,VK_SHARING_MODE_EXCLUSIVE,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,VK_IMAGE_TILING_OPTIMAL,m_SwapChain->GetExtent().width,m_SwapChain->GetExtent().height);
        }
        CreateFrameBuffers();


        CreateCommandBuffers();
        CreateSamaphore();
     

        m_VertexCount = desc.VertexCountPerDrawCall;
        m_VertexBuffer =new VertexBuffer(m_Device,m_PhysicalDevice,sizeof(Vertex)*m_VertexCount);
        m_Vertices = new Vertex[m_VertexCount];
        

        uint32_t* Indices = new uint32_t[(uint32_t)(m_VertexCount*1.5)];
        uint32_t offset{};
        for(int i =0;i < m_VertexCount*1.5;i+=6){
                
           Indices[i] = offset;
           Indices[i+1] = offset+1;
           Indices[i+2] = offset+2;

            Indices[i+3] = offset+2;
           Indices[i+4] = offset+3;
           Indices[i+5] = offset;
           offset+=4;
        }


     


        m_IndexBuffer.Init(m_Device,m_PhysicalDevice,m_VertexCount*1.5*sizeof(uint32_t));
        m_IndexBuffer.UploadToBuffer(m_Device,Indices,m_VertexCount*1.5*sizeof(uint32_t));

        //VkFormat format2 = Core::ChooseBestFormat(m_PhysicalDevice,{ VK_FORMAT_R32G32_UINT },VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

   // m_FrameImageIndexed = new Image(m_PhysicalDevice,m_Device,format2,VK_SHARING_MODE_EXCLUSIVE,VK_IMAGE_USAGE_STORAGE_BIT,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT           ,VK_IMAGE_TILING_OPTIMAL,m_SwapChain->GetExtent().width,m_SwapChain->GetExtent().height);

        m_PickingImageBuffer = new Buffer(m_Device,VK_BUFFER_USAGE_TRANSFER_DST_BIT,VK_SHARING_MODE_EXCLUSIVE,m_PhysicalDevice,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,m_ColorAttachments[0].GetSize());
 }
    void Renderer::BeginFrame(glm::mat4 viewproj){
               //RecordCommands(m_VertexCount,0);

            m_DrawCallCount =0;
            m_CameraViewProj =viewproj;

            VkSwapchainKHR swapchain = m_SwapChain->GetSwapChain();
        

             vkWaitForFences(m_Device,1,&m_DrawFences[m_CurrentFrame],true,std::numeric_limits<uint64_t>::max());

        vkAcquireNextImageKHR(m_Device,m_SwapChain->GetSwapChain(),std::numeric_limits<uint64_t>::max(),m_ImageAvailS[m_CurrentFrame],nullptr,&m_ImageIndex);
        static bool Started{false};
        if(Started)
        {



          //  Float2 a=m_PickingImageBuffer->ReadPixel(0   ,0,500,500);
           // Core::Log(ErrorType::Error,"Color ",a.x," ",a.y);
        }
 Started = true;
    }
    void Renderer::Flush(bool LastFrame){
    
         vkWaitForFences(m_Device,1,&m_DrawFences[m_CurrentFrame],true,std::numeric_limits<uint64_t>::max());
        vkResetFences(m_Device,1,&m_DrawFences[m_CurrentFrame]); 

           m_UniformBuffers[0].UploadToBuffer(m_Device,&m_CameraViewProj,sizeof(glm::mat4));

        m_VertexBuffer->UploadToBuffer(m_Device,m_Vertices,sizeof(Vertex)*m_VertexPointer);
     
       RecordCommands(m_VertexCount,0);



        VkCommandBuffer commandsbuffers[]={m_CommandBuffers[m_ImageIndex],m_TransferCommandBuffer};

       VkPipelineStageFlags waitstages[]= {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitinfo{};
        submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        if(m_DrawCallCount==0){
        submitinfo.waitSemaphoreCount=1;
        submitinfo.pWaitSemaphores = &m_ImageAvailS[m_CurrentFrame];
        submitinfo.pWaitDstStageMask=waitstages;
        }
        submitinfo.commandBufferCount = 1;
        submitinfo.pCommandBuffers = commandsbuffers;
        if(LastFrame){
        submitinfo.signalSemaphoreCount = 1;
        submitinfo.pSignalSemaphores = &m_RenderFinishedS[m_CurrentFrame];
        }
        VkResult result= vkQueueSubmit(m_GraphicsQ,1,&submitinfo,m_DrawFences[m_CurrentFrame]);
        vkQueueWaitIdle(m_GraphicsQ);
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to submit queue.");

 
            m_VertexPointer=0;
            m_DrawCallCount++;
            
    }
    void Renderer::EndFrame(){

        Flush(true);

            VkSwapchainKHR swapchain = m_SwapChain->GetSwapChain();
     
        
       
                    VkPresentInfoKHR presentinfo{};
        presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentinfo.waitSemaphoreCount=1;
        presentinfo.pWaitSemaphores = &m_RenderFinishedS[m_CurrentFrame ];
        presentinfo.swapchainCount =1;
        presentinfo.pSwapchains = &swapchain;
        presentinfo.pImageIndices = &m_ImageIndex;
    

          VkResult result = vkQueuePresentKHR(m_PresentationQ,&presentinfo);
      
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to queue present.");

     
       m_CurrentFrame=(m_CurrentFrame+1)%MAX_FRAME_DRAWS;


    }

    void Renderer::DrawQuad(Float3 Position,Float4 Color,Float2 Size,Image* image,uint64_t ID){
        
        if(m_VertexPointer+4> m_VertexCount)
            Flush(false);


        m_Vertices[m_VertexPointer].Position ={Position.x-Size.x,Position.y-Size.y} ;
        m_Vertices[m_VertexPointer+1].Position = {Position.x-Size.x,Position.y+Size.y};
        m_Vertices[m_VertexPointer+2].Position = {Position.x+Size.x,Position.y+Size.y};
        m_Vertices[m_VertexPointer+3].Position = {Position.x+Size.x,Position.y-Size.y};




        m_Vertices[m_VertexPointer].Color = Color;
        m_Vertices[m_VertexPointer+1].Color = Color;
        m_Vertices[m_VertexPointer+2].Color = Color;
        m_Vertices[m_VertexPointer+3].Color = Color;



        m_Vertices[m_VertexPointer].ID = ID;
        m_Vertices[m_VertexPointer+1].ID = ID;
        m_Vertices[m_VertexPointer+2].ID = ID;
        m_Vertices[m_VertexPointer+3].ID = ID;



        m_VertexPointer+=4;

       

    }

    void Renderer::DrawParticle()
    {
    }


    void Renderer::CreateInstance(){


 

        VkApplicationInfo appinfo{};
        appinfo.sType= VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appinfo.pApplicationName = "Vulkan App";
        appinfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        appinfo.pEngineName = "No engine";
        appinfo.engineVersion = VK_MAKE_VERSION(1,0,0);
        appinfo.apiVersion = VK_API_VERSION_1_2;

        std::vector<const char*> instanceExtensiosn{};
        uint32_t ExtensionCount{};
        const char** glfwExtensions{};


        glfwExtensions = glfwGetRequiredInstanceExtensions(&ExtensionCount);


        for(int i =0;i < ExtensionCount;i++){
            instanceExtensiosn.push_back(glfwExtensions[i]);
        }
        instanceExtensiosn.push_back("VK_EXT_debug_utils");
        
        if(!IsExtensionsSupported(instanceExtensiosn))
            return;

        std::vector<const char*> LayerNames{"VK_LAYER_KHRONOS_validation"};

        VkInstanceCreateInfo createinfo{};
        createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createinfo.pApplicationInfo= &appinfo;
        createinfo.enabledExtensionCount = instanceExtensiosn.size();
        createinfo.ppEnabledExtensionNames = instanceExtensiosn.data();
        //Add check if layers is supported.
        createinfo.ppEnabledLayerNames  = LayerNames.data();
        createinfo.enabledLayerCount = 1;


        VkResult result = vkCreateInstance(&createinfo,nullptr,&m_Instance);

        if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error ,"Failed to create instance");
    }
    void Renderer::CreateSamaphore(){
          VkSemaphoreCreateInfo semaphoreinfo{};
          semaphoreinfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            m_ImageAvailS.resize(MAX_FRAME_DRAWS);
            m_RenderFinishedS.resize(MAX_FRAME_DRAWS);
            m_DrawFences.resize(MAX_FRAME_DRAWS);
        
        VkFenceCreateInfo fenceinfo{};
        fenceinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceinfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(int i =0;i < MAX_FRAME_DRAWS;i++){
          VkResult result = vkCreateSemaphore(m_Device,&semaphoreinfo,nullptr,&m_ImageAvailS[i]);
          if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create semaphore.");

           result = vkCreateSemaphore(m_Device,&semaphoreinfo,nullptr,&m_RenderFinishedS[i]);
          if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create semaphore.");

            result =vkCreateFence(m_Device,&fenceinfo,nullptr,&m_DrawFences[i]);
            if(result != VK_SUCCESS)
                Core::Log(ErrorType::Error,"Failed to create fence.");

    }
    
    }

    void Renderer::CreateLogicalDevice(){
  

        QueueFamilies families =GetQueueFamilies(m_PhysicalDevice);
        std::vector<VkDeviceQueueCreateInfo> queueInfos{};
        std::set<int> QueueFamilyIndices{families.Graphics,families.Presentation};

        for(int Index: QueueFamilyIndices){
        VkDeviceQueueCreateInfo  queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = Index;
        queueInfo.queueCount = 1;
        
        float Priority = 1.0f;
        queueInfo.pQueuePriorities = &Priority;

            queueInfos.push_back(queueInfo);
        }



        VkDeviceCreateInfo devicecreateinfo{};
        devicecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devicecreateinfo.queueCreateInfoCount = queueInfos.size();
        devicecreateinfo.pQueueCreateInfos = queueInfos.data();
        devicecreateinfo.enabledLayerCount = 0;
        devicecreateinfo.ppEnabledExtensionNames = nullptr;
        devicecreateinfo.enabledExtensionCount = g_DeviceExtensions.size();
        devicecreateinfo.ppEnabledExtensionNames = g_DeviceExtensions.data();


        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.independentBlend = true;

        devicecreateinfo.pEnabledFeatures = &deviceFeatures;
        

        VkResult result = vkCreateDevice(m_PhysicalDevice,&devicecreateinfo,nullptr,&m_Device);
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create Device.");


            vkGetDeviceQueue(m_Device,families.Graphics,0,&m_GraphicsQ);
            vkGetDeviceQueue(m_Device,families.Presentation,0,&m_PresentationQ);

    }
    void Renderer::CreatePipeline(){

        m_RenderPass = CreateRenderPass(m_Device,m_SwapChain->GetFormat());

        Shader vertex("C:/Repos/VulkanEngine/Shaders/Vertex.spv",m_Device);
        Shader fragment("C:/Repos/VulkanEngine/Shaders/Fragment.spv",m_Device);

        VkPipelineShaderStageCreateInfo vertexstage{};
        vertexstage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexstage.module  = vertex.GetShaderModule();
        vertexstage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexstage.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentstage{};
        fragmentstage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentstage.module  = fragment.GetShaderModule();
        fragmentstage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentstage.pName = "main";

        VkPipelineShaderStageCreateInfo stages[] = {vertexstage,fragmentstage};

        VkVertexInputBindingDescription vertexinputdesc{};        
        vertexinputdesc.stride = sizeof(Vertex);
        vertexinputdesc.binding=0;
        vertexinputdesc.inputRate =VK_VERTEX_INPUT_RATE_VERTEX;


        VkVertexInputAttributeDescription vertexinputattr[3];

        vertexinputattr[0].binding = 0;
        vertexinputattr[0].location = 0;
        vertexinputattr[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexinputattr[0].offset = offsetof(Vertex,Position);

        vertexinputattr[1].binding = 0;
        vertexinputattr[1].location = 1;
        vertexinputattr[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexinputattr[1].offset = offsetof(Vertex,Color);

        vertexinputattr[2].binding = 0;
        vertexinputattr[2].location = 2;
        vertexinputattr[2].format = VK_FORMAT_R32G32_UINT;
        vertexinputattr[2].offset = offsetof(Vertex, ID);




        VkPipelineVertexInputStateCreateInfo vertexinputstate{};
        vertexinputstate.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexinputstate.vertexAttributeDescriptionCount = 3;
        vertexinputstate.vertexBindingDescriptionCount = 1;
        vertexinputstate.pVertexBindingDescriptions =&vertexinputdesc;
        vertexinputstate.pVertexAttributeDescriptions = vertexinputattr;


        VkPipelineInputAssemblyStateCreateInfo inputassassembly{};
        inputassassembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputassassembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputassassembly.primitiveRestartEnable = false;
       


        VkViewport viewport{};
        viewport.x =0.0f;
        viewport.y = 0.0f;
        viewport.width = m_SwapChain->GetExtent().width;
        viewport.height = m_SwapChain->GetExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D Scissor{};
        Scissor.offset = {0,0};
        Scissor.extent=m_SwapChain->GetExtent();

        VkPipelineViewportStateCreateInfo viewportinfo{};
        viewportinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportinfo.viewportCount = 1;
        viewportinfo.pViewports = &viewport;
        viewportinfo.scissorCount = 1;
        viewportinfo.pScissors = &Scissor;

        VkPipelineRasterizationStateCreateInfo rasterizationstate{};
        rasterizationstate.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationstate.depthClampEnable = false;
        rasterizationstate.rasterizerDiscardEnable = false;
        rasterizationstate.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationstate.lineWidth = 1.0f;
        rasterizationstate.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationstate.frontFace=VK_FRONT_FACE_CLOCKWISE;
        rasterizationstate.depthBiasEnable = false;

        VkPipelineMultisampleStateCreateInfo multisampleinfo{};
        multisampleinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleinfo.sampleShadingEnable = false;
        multisampleinfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;



        VkPipelineColorBlendAttachmentState blendattachment{};
        blendattachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT|VK_COLOR_COMPONENT_G_BIT|VK_COLOR_COMPONENT_B_BIT|VK_COLOR_COMPONENT_A_BIT;
        blendattachment.blendEnable = true;
        
        blendattachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendattachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendattachment.colorBlendOp = VK_BLEND_OP_ADD;
        blendattachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendattachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendattachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendAttachmentState blendattachment2{};
        blendattachment2.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT  ;
        blendattachment2.blendEnable = false;

        blendattachment2.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendattachment2.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendattachment2.colorBlendOp = VK_BLEND_OP_ADD;
        blendattachment2.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendattachment2.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendattachment2.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendAttachmentState blendattachments[]={blendattachment,blendattachment2 };
        
        VkPipelineColorBlendStateCreateInfo blendstate{};
        blendstate.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        blendstate.logicOpEnable = false;
        blendstate.pAttachments = blendattachments;
        blendstate.attachmentCount = 2;

        VkPipelineLayoutCreateInfo pipelinelayout{};
        pipelinelayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelinelayout.setLayoutCount =1;
        pipelinelayout.pSetLayouts = m_DescriptorLayout.GetDescriptorLayout();
        pipelinelayout.pushConstantRangeCount=0;


        VkResult result = vkCreatePipelineLayout(m_Device,&pipelinelayout,nullptr,&m_PipelineLayout);
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create pipeline layout.");


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
        pipeline.layout = m_PipelineLayout;
        pipeline.renderPass = m_RenderPass;
        pipeline.subpass = 0;

        pipeline.basePipelineIndex = -1;

         result = vkCreateGraphicsPipelines(m_Device,nullptr,1,&pipeline,nullptr,&m_Pipeline);
         if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create pipeline.");

         

        




        
    }
    void Renderer::CreateFrameBuffers(){

        m_FrameBuffers.resize(m_SwapChain->GetSwapChainImageCount());
        for(int i =0;i < m_FrameBuffers.size();i++){
        Image images[]= {*m_SwapChain->GetSwapChainImage(i),m_ColorAttachments[i]};
                m_FrameBuffers[i].Init(m_Device,m_SwapChain,m_RenderPass,images,2);
        }


    }
    void Renderer::RecordCommands(uint32_t VertexCount,uint32_t IndexCount){
        VkCommandBufferBeginInfo bufferbegininfo{};
        bufferbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkClearValue ClearColor[] ={{1.0f,0.0f,0.0f,1.0f},{0.0f,0.0f}}; 


        VkRenderPassBeginInfo renderpassbegininfo{};
        renderpassbegininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderpassbegininfo.renderPass =m_RenderPass;
        renderpassbegininfo.renderArea.offset={0,0};
        renderpassbegininfo.renderArea.extent = m_SwapChain->GetExtent();
        renderpassbegininfo.pClearValues = ClearColor;
       renderpassbegininfo.clearValueCount = 2;
       

        for(int i=0;i < m_CommandBuffers.size();i++){
    
             
        renderpassbegininfo.framebuffer = m_FrameBuffers[i].GetFrameBuffer(i);
            //vkWaitForFences(m_Device,1,&m_DrawFences[m_CurrentFrame],true,std::numeric_limits<uint64_t>::max());
                vkResetCommandBuffer(m_CommandBuffers[i],VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
             VkResult result = vkBeginCommandBuffer(m_CommandBuffers[i],&bufferbegininfo);
             VkBufferImageCopy  copyregion{};
             copyregion.bufferImageHeight = m_ColorAttachments[0].GetExtent().height;
             copyregion.bufferRowLength = m_ColorAttachments[0].GetExtent().width;
             copyregion.bufferOffset = 0;

             copyregion.imageExtent = { m_ColorAttachments[0].GetExtent().width,m_ColorAttachments[0].GetExtent().height,1 };
             copyregion.imageOffset = { 0,0,0 };
             copyregion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
             copyregion.imageSubresource.mipLevel = 0;
             copyregion.imageSubresource.baseArrayLayer = 0;
             copyregion.imageSubresource.layerCount = 1;




             if (m_CurrentFrame == 1)
                 vkCmdCopyImageToBuffer(m_CommandBuffers[i], m_ColorAttachments[0].GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, *m_PickingImageBuffer->GetBuffer(), 1, &copyregion);


             if(result != VK_SUCCESS)
                 {
                        Core::Log(ErrorType::Error,"Failed to begin recording command buffer.");
                        return ;
                 }   
                     
         
                vkCmdBindPipeline(m_CommandBuffers[i],VK_PIPELINE_BIND_POINT_GRAPHICS,m_Pipeline);
                vkCmdBeginRenderPass(m_CommandBuffers[i],&renderpassbegininfo,VK_SUBPASS_CONTENTS_INLINE);

                
                VkDeviceSize offset{};
                vkCmdBindVertexBuffers(m_CommandBuffers[i],0,1,m_VertexBuffer->GetBuffer(),&offset);

                vkCmdBindIndexBuffer(m_CommandBuffers[i],*m_IndexBuffer.GetBuffer(),offset,VK_INDEX_TYPE_UINT32);

                vkCmdBindDescriptorSets(m_CommandBuffers[i],VK_PIPELINE_BIND_POINT_GRAPHICS,m_PipelineLayout,0,1,m_DescriptorLayout.GetDescriptorSet(),0,nullptr);

                vkCmdDrawIndexed(m_CommandBuffers[i],VertexCount*1.5,1,0,0,0);

                

                vkCmdEndRenderPass(m_CommandBuffers[i]);
                
             

            
   



            VkCommandBufferBeginInfo begininfo{};
            begininfo.sType=VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

          // vkBeginCommandBuffer(m_TransferCommandBuffer,&begininfo);   
           
            // vkCmdCopyImage(m_TransferCommandBuffer,m_ColorAttachments[0].GetImage(),VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,m_FrameImageIndexed->GetImage(),VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&copyregion);
         //  vkEndCommandBuffer(m_TransferCommandBuffer);
           result = vkEndCommandBuffer(m_CommandBuffers[i]);
           if (result != VK_SUCCESS)
               Core::Log(ErrorType::Error, "Failed to end command buffer.");

        }

             }
    void Renderer::CreateCommandBuffers(){

        m_GraphicsPool.Init(m_Device,GetQueueFamilies(m_PhysicalDevice).Graphics);


        m_CommandBuffers.resize(m_SwapChain->GetSwapChainImageCount());
        

        VkCommandBufferAllocateInfo allocinfo{};
        allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocinfo.commandPool = m_GraphicsPool.GetCommandPool();
        allocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocinfo.commandBufferCount =m_SwapChain->GetSwapChainImageCount();

        VkResult result = vkAllocateCommandBuffers(m_Device,&allocinfo,m_CommandBuffers.data());
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create command buffers.");



               VkCommandBufferAllocateInfo allocinfo2{};
        allocinfo2.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocinfo2.commandPool = m_GraphicsPool.GetCommandPool();
        allocinfo2.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocinfo2.commandBufferCount =1;

         result = vkAllocateCommandBuffers(m_Device,&allocinfo2,&m_TransferCommandBuffer);
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create command buffers.");




    }
    void Renderer::GetPhysicalDevice(){
            uint32_t DeviceCount{};
 
            std::vector<VkPhysicalDevice> Devices{};
         
            vkEnumeratePhysicalDevices(m_Instance,&DeviceCount,nullptr);
            Devices.resize(DeviceCount);
            vkEnumeratePhysicalDevices(m_Instance,&DeviceCount,Devices.data());
   
            for(int i =0;i < DeviceCount;i++){
                    
                    if(IsDeviceSiutable(Devices[i])){
                        m_PhysicalDevice = Devices[i];

                        return;
                    }

            }

           Core::Log(ErrorType::Info,"Failed to get physical device.");
 
    }

    bool Renderer::IsDeviceSiutable(VkPhysicalDevice device){

            bool Ret{true};

         QueueFamilies Fam=   GetQueueFamilies(device);
         

    SwapChainDetails swapChainDetails = SwapChain::GetSwapChainCapabilities(device,m_Surface);
    if(!Fam.IsValid()){
            Core::Log(ErrorType::Error,"Device doesn't support neccesarry queue families.");

        Ret=false;
    }
    if(!CheckDeviceExtensionSupport(device)){
            Core::Log(ErrorType::Error,"Device doesn't support neccesarry extensions.");

            Ret= false;

    }

    if(!SwapChain::IsSwapChainValid(swapChainDetails))
    {
            Core::Log(ErrorType::Error,"SwapChain doesn't support neccesarry features.");
            Ret= false;
    }
        return Ret;
    }
    bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device){
                uint32_t ExtCount{};
                std::vector<VkExtensionProperties> DeviceExts{};

                vkEnumerateDeviceExtensionProperties(device,nullptr,&ExtCount,nullptr);
                DeviceExts.resize(ExtCount);
                vkEnumerateDeviceExtensionProperties(device,nullptr,&ExtCount,DeviceExts.data());

                if(ExtCount ==0)
                 Core::Log(ErrorType::Error,"This device doesn't support any extensions.");

                for(int i =0;i < g_DeviceExtensions.size();i++){
                        bool HasExtension{};
                    for(int j =0;j < ExtCount;j++){
                        if(strcmp(g_DeviceExtensions[i],DeviceExts[j].extensionName))
                        HasExtension = true;
                    }
                    if(!HasExtension){

                        return false;
                        Core::Log(ErrorType::Error,"Device doesn't support the required extensions");
                    }
                }
        return true;
    }


    QueueFamilies Renderer::GetQueueFamilies(VkPhysicalDevice device){
            QueueFamilies Families{};

            uint32_t QueueFamCount{};
            std::vector<VkQueueFamilyProperties> Properties{};

            vkGetPhysicalDeviceQueueFamilyProperties(device,&QueueFamCount,nullptr);
            Properties.resize(QueueFamCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device,&QueueFamCount,Properties.data());
         

            for(int i=0;i < QueueFamCount;i++){
                if(Properties[i].queueCount >0&&Properties[i].queueFlags& VK_QUEUE_GRAPHICS_BIT);
                {

                    Families.Graphics = i;
                }
                VkBool32 Presentation = false;
                
                vkGetPhysicalDeviceSurfaceSupportKHR(device,i,m_Surface,&Presentation);
                if(Properties[i].queueCount >0&&Presentation )
                    Families.Presentation = i;
                
                Core::Log(ErrorType::Error,Families.Graphics," ",Families.Presentation);

                if(Families.IsValid()){

                    return Families;
                }
            }
                Families.Presentation = 0;
            return Families;
    }
    bool Renderer::IsExtensionsSupported(std::vector<const char*> extensions){
        uint32_t SupportedExCount{};
        std::vector<VkExtensionProperties> Props{};

        vkEnumerateInstanceExtensionProperties(nullptr,&SupportedExCount,nullptr);
        Props.resize(SupportedExCount);
        vkEnumerateInstanceExtensionProperties(nullptr,&SupportedExCount,Props.data());


        for(int i =0; i <extensions.size();i++){
            bool HasExt{false};

            for(int j =0;j < Props.size();j++){

                if(strcmp(extensions[i],Props[j].extensionName)==0)
                    {
                        HasExt = true;
                        break;
                    }

            }
            if(!HasExt)
            return false;
        }
        
        return true;



    }
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) 
       {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
          return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
     } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
     }   
       } 
    void  Renderer::CreateSurface(GLFWwindow* window,VkSurfaceKHR* surface){
             VkResult result= glfwCreateWindowSurface(m_Instance,window,nullptr,surface);
            if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create surface.");
    }
    void Renderer::CreateDebugger(){

        VkDebugUtilsMessengerCreateInfoEXT creatinfo{};
        creatinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        creatinfo.messageSeverity  = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        creatinfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        creatinfo.pfnUserCallback = VulkanDebugCallBack;
        creatinfo.pUserData = nullptr;
        




        VkResult result  = CreateDebugUtilsMessengerEXT(m_Instance,&creatinfo,nullptr,&m_Messenger);
        if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create debug messenger.");

    }
    void Renderer::Statistics(){

        Core::Log(ErrorType::Info,"Draw call count",m_DrawCallCount);
    }



Renderer::~Renderer(){
    vkDeviceWaitIdle(m_Device);


    vkDestroySurfaceKHR(m_Instance,m_Surface,nullptr);
    vkDestroyDevice(m_Device,nullptr);
    vkDestroyInstance(m_Instance,nullptr);
}


static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageseverity,VkDebugUtilsMessageTypeFlagsEXT messagetype,const VkDebugUtilsMessengerCallbackDataEXT* pcallbackdata,void* puserData ){



        switch(messageseverity){
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:{
                 Core::Log(ErrorType::Info,pcallbackdata->pMessage);

                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:{
                 Core::Log(ErrorType::Error,pcallbackdata->pMessage);

                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:{
                 Core::Log(ErrorType::Warning,pcallbackdata->pMessage);

                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:{
                 Core::Log(ErrorType::Info,pcallbackdata->pMessage);

                    break;
            }
        }

    return VK_FALSE;
}