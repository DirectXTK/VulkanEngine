#include "Renderer.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Device.h"
 Renderer::Renderer(RendererDesc desc,GLFWwindow* window){
   m_Window = window;
   m_ClearColor = desc.ClearColor;
    //m_VertexBufferSize = 3*100;
   //Create vulkan instance
   InstanceDesc instancedesc{};
   instancedesc.ApiVersion = VK_API_VERSION_1_2;
   instancedesc.ValidationLayersEnabled = true;
     m_Instance = VulkanInstance::CreateInstance(instancedesc,&m_Messenger);

    CreateSurface(window,&m_Surface);

    m_PhysicalDevice = VulkanInstance::GetPhysicalDevice(m_Instance,m_Surface);
    m_SwapChainDetails = SwapChain::GetSwapChainCapabilities(m_PhysicalDevice, m_Surface);

    m_QueueFamilies = VulkanInstance::GetQueueFamilies(m_PhysicalDevice, m_Surface);
    m_Device = LogicalDevice::CreateLogicalDevice(m_PhysicalDevice, m_QueueFamilies);
    LogicalDevice::GetQueues(m_Device, m_QueueFamilies, &m_GraphicsQ, &m_PresentationQ);



     m_SwapChain = new SwapChain(m_Instance,m_PhysicalDevice,m_Device,m_Surface, m_QueueFamilies);
    m_SwapChainDetails = m_SwapChain->GetSwapChainCapabilities();

        m_SwapChain->CreateSwapChain();

      
             //Matrices
        m_UniformBuffers = new UniformBuffer[MAX_FRAME_DRAWS];
        for(int i =0;i < MAX_FRAME_DRAWS;i++){
        m_UniformBuffers[i].Init(m_Device,m_PhysicalDevice,sizeof(glm::mat4));
     

        }
          m_DescriptorLayout.Init(m_Device);
        m_DescriptorLayout.WriteTo(m_Device,*m_UniformBuffers[0].GetBuffer(),sizeof(glm::mat4));

        m_RenderPass = Pipeline::CreateRenderPass(m_Device, m_SwapChain->GetFormat());
        m_PipelineLayout = Pipeline::CreatePipelineLayout(m_Device,m_DescriptorLayout.GetDescriptorLayout());

        PipelineDesc pipelineDesc{};
        pipelineDesc.RenderPass = m_RenderPass;
        pipelineDesc.PipelineLayout = m_PipelineLayout;
        pipelineDesc.VertexStageInputCount = 3;
        pipelineDesc.VertexInputStride = sizeof(Vertex);
        pipelineDesc.VertexStageInput = new VertexStageInputAttrib[pipelineDesc.VertexStageInputCount];
        pipelineDesc.VertexStageInput[0] = { VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex,Position),0,0 };
        pipelineDesc.VertexStageInput[1] = { VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(Vertex,Color),1,0 };
        pipelineDesc.VertexStageInput[2] = { VK_FORMAT_R32G32_UINT,offsetof(Vertex,ID),2,0 };

        pipelineDesc.Viewport = { 0,0,(float)m_SwapChain->GetExtent().width,(float)m_SwapChain->GetExtent().height,0.0f,1.0f };


       m_Pipeline =  Pipeline::CreatePipeline(pipelineDesc, m_Device);

        VkFormat format = Core::ChooseBestFormat(m_PhysicalDevice,{ VK_FORMAT_R32G32_UINT },VK_IMAGE_TILING_OPTIMAL,VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);

         m_ColorAttachments.resize(MAX_FRAME_DRAWS);
         for(int i =0;i < m_ColorAttachments.size();i++){

              m_ColorAttachments[i]=Image(m_PhysicalDevice,m_Device,format,VK_SHARING_MODE_EXCLUSIVE,VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,VK_IMAGE_TILING_OPTIMAL,m_SwapChain->GetExtent().width,m_SwapChain->GetExtent().height);
        }
        CreateFrameBuffers();


        CreateCommandBuffers();
        CreateSamaphore();
     

        m_VertexCount = desc.VertexCountPerDrawCall;
        BufferDesc VertexBufferDesc{};
        VertexBufferDesc.SizeBytes = desc.VertexCountPerDrawCall*sizeof(Vertex);
        VertexBufferDesc.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT| VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        VertexBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
        VertexBufferDesc.Physdevice = m_PhysicalDevice;
        VertexBufferDesc.Device = m_Device;
        VertexBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ;


        m_VertexBuffers.push_back(new Buffer(VertexBufferDesc));

       // VertexBufferDesc.SizeBytes = desc.VertexCountPerDrawCall * sizeof(Vertex)*20000;

        //Buffer* vertexdwadad = new Buffer(VertexBufferDesc);

        m_Vertices = new Vertex[m_VertexCount];

        BufferDesc StaggingBufferDesc{};
        StaggingBufferDesc.SizeBytes = desc.VertexCountPerDrawCall * sizeof(Vertex);
        StaggingBufferDesc.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        StaggingBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
        StaggingBufferDesc.Physdevice = m_PhysicalDevice;
        StaggingBufferDesc.Device = m_Device;
        StaggingBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        m_StaggingBuffers.push_back(new Buffer(StaggingBufferDesc));
        //temp

        m_Indices = new uint32_t[(uint32_t)(m_VertexCount*1.5)];
        uint32_t offset{};
        for(int i =0;i < m_VertexCount*1.5;i+=6){
                
            m_Indices[i] = offset;
            m_Indices[i+1] = offset+1;
            m_Indices[i+2] = offset+2;

            m_Indices[i+3] = offset+2;
            m_Indices[i+4] = offset+3;
            m_Indices[i+5] = offset;
           offset+=4;
        }


        BufferDesc IndexBufferDesc{};
        IndexBufferDesc.SizeBytes = uint64_t(desc.VertexCountPerDrawCall * sizeof(uint32_t)*1.5f);
        IndexBufferDesc.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        IndexBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
        IndexBufferDesc.Physdevice = m_PhysicalDevice;
        IndexBufferDesc.Device = m_Device;
        IndexBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;




        m_IndexBuffers.push_back(new Buffer(IndexBufferDesc));
        m_IndexBuffers[0]->UploadToBuffer(m_Device, m_Indices, uint64_t(m_VertexCount * 1.5 * sizeof(uint32_t)));

        //VkFormat format2 = Core::ChooseBestFormat(m_PhysicalDevice,{ VK_FORMAT_R32G32_UINT },VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

   // m_FrameImageIndexed = new Image(m_PhysicalDevice,m_Device,format2,VK_SHARING_MODE_EXCLUSIVE,VK_IMAGE_USAGE_STORAGE_BIT,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT           ,VK_IMAGE_TILING_OPTIMAL,m_SwapChain->GetExtent().width,m_SwapChain->GetExtent().height);

        BufferDesc PickingImageBufferDesc{};
        PickingImageBufferDesc.SizeBytes = m_ColorAttachments[0].GetSize();
        PickingImageBufferDesc.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        PickingImageBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
        PickingImageBufferDesc.Physdevice = m_PhysicalDevice;
        PickingImageBufferDesc.Device = m_Device;
        PickingImageBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ;


        m_PickingImageBuffer = new Buffer(PickingImageBufferDesc);
 }
    void Renderer::BeginFrame(glm::mat4 viewproj){
               //RecordCommands(m_VertexCount,0);

            m_DrawCallCount =0;
            m_CameraViewProj =viewproj;

            VkSwapchainKHR swapchain = m_SwapChain->GetSwapChain();
        

             vkWaitForFences(m_Device,1,&m_DrawFences[m_CurrentFrame],true,std::numeric_limits<uint64_t>::max());

        vkAcquireNextImageKHR(m_Device,m_SwapChain->GetSwapChain(),std::numeric_limits<uint64_t>::max(),m_ImageAvailS[m_CurrentFrame],nullptr,&m_ImageIndex);


       
      




           
            //vkWaitForFences(m_Device,1,&m_DrawFences[m_CurrentFrame],true,std::numeric_limits<uint64_t>::max());

            StartRecordingCommands();

           
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
                vkCmdCopyImageToBuffer(m_CurrentCommandBuffer, m_ColorAttachments[0].GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, *m_PickingImageBuffer->GetBuffer(), 1, &copyregion);
            
          



    }
    void Renderer::CreateNewBufferForBatch()
    {


        m_VertexBuffers.push_back(new Buffer(m_VertexBuffers[0]->GetBufferDesc()));
        m_StaggingBuffers.push_back(new Buffer(m_StaggingBuffers[0]->GetBufferDesc()));


       // m_IndexBuffers.push_back(new Buffer(m_IndexBuffers[0]->GetBufferDesc()));
      //  m_IndexBuffers[m_IndexBuffers.size() - 1]->UploadToBuffer(m_Device, m_Indices, uint64_t(m_VertexCount * 1.5f * sizeof(uint32_t)));

        
    }
    void Renderer::Flush(bool LastFrame){

        m_DrawCommands.push_back({ m_VertexPointer,m_DrawCallCount });

        if (m_DrawCallCount == m_VertexBuffers.size())
            CreateNewBufferForBatch();



           m_UniformBuffers[0].UploadToBuffer(m_Device,&m_CameraViewProj,sizeof(glm::mat4));

           m_StaggingBuffers[m_DrawCallCount]->UploadToBuffer(m_Device, m_Vertices, sizeof(Vertex) * m_VertexPointer);

           VkBufferCopy region{};
           region.size = sizeof(Vertex) * m_VertexPointer;

           if (m_VertexPointer != 0)
           {
               vkCmdCopyBuffer(m_CurrentCommandBuffer, *m_StaggingBuffers[m_DrawCallCount]->GetBuffer(), *m_VertexBuffers[m_DrawCallCount]->GetBuffer(), 1, &region);

           }
          




        
 
            m_VertexPointer=0;
            m_DrawCallCount++;
            
    }
    void Renderer::EndFrame(){


        Flush(true);
        DrawBatch();


        StopRecordingCommands();

        vkWaitForFences(m_Device, 1, &m_DrawFences[m_CurrentFrame], true, std::numeric_limits<uint64_t>::max());
        vkResetFences(m_Device, 1, &m_DrawFences[m_CurrentFrame]);

       
            VkSwapchainKHR swapchain = m_SwapChain->GetSwapChain();
     
            VkPipelineStageFlags waitstages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            VkCommandBuffer commandsbuffers[] = { m_CommandBuffers[m_CurrentFrame],m_TransferCommandBuffer };


            VkSubmitInfo submitinfo{};
            submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitinfo.waitSemaphoreCount = 1;
            submitinfo.pWaitSemaphores = &m_ImageAvailS[m_CurrentFrame];
            submitinfo.pWaitDstStageMask = waitstages;
            submitinfo.commandBufferCount = 1;
            submitinfo.pCommandBuffers = commandsbuffers;
            submitinfo.signalSemaphoreCount = 1;
            submitinfo.pSignalSemaphores = &m_RenderFinishedS[m_CurrentFrame];
            VkResult result = vkQueueSubmit(m_GraphicsQ, 1, &submitinfo, m_DrawFences[m_CurrentFrame]);
            vkQueueWaitIdle(m_GraphicsQ);
            if (result != VK_SUCCESS)
                Core::Log(ErrorType::Error, "Failed to submit queue.");


        
        
       
                    VkPresentInfoKHR presentinfo{};
        presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentinfo.waitSemaphoreCount=1;
        presentinfo.pWaitSemaphores = &m_RenderFinishedS[m_CurrentFrame ];
        presentinfo.swapchainCount =1;
        presentinfo.pSwapchains = &swapchain;
        presentinfo.pImageIndices = &m_ImageIndex;
    

            result = vkQueuePresentKHR(m_PresentationQ,&presentinfo);
      
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

    void Renderer::CreateFrameBuffers(){

        m_FrameBuffers.resize(MAX_FRAME_DRAWS);
        for(int i =0;i < m_FrameBuffers.size();i++){
        Image images[]= {*m_SwapChain->GetSwapChainImage(i),m_ColorAttachments[i]};
                m_FrameBuffers[i].Init(m_Device,m_SwapChain,m_RenderPass,images,2);
        }


    }

    void Renderer::CreateCommandBuffers(){

        m_GraphicsPool.Init(m_Device,m_QueueFamilies.Graphics);


        m_CommandBuffers.resize(MAX_FRAME_DRAWS);
        

        VkCommandBufferAllocateInfo allocinfo{};
        allocinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocinfo.commandPool = m_GraphicsPool.GetCommandPool();
        allocinfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocinfo.commandBufferCount = MAX_FRAME_DRAWS;

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
   
    void  Renderer::CreateSurface(GLFWwindow* window,VkSurfaceKHR* surface){
             VkResult result= glfwCreateWindowSurface(m_Instance,window,nullptr,surface);
            if(result != VK_SUCCESS)
            Core::Log(ErrorType::Error,"Failed to create surface.");
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

void Renderer::StartRecordingCommands()
{
 
    VkCommandBufferBeginInfo bufferbegininfo{};
    bufferbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentFrame];


    vkResetCommandBuffer(m_CurrentCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    vkBeginCommandBuffer(m_CurrentCommandBuffer, &bufferbegininfo);
    vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);


}

void Renderer::StopRecordingCommands()
{

    vkEndCommandBuffer(m_CurrentCommandBuffer);
    m_DrawCommands.resize(0);
}

void Renderer::DrawBatch()
{
    VkClearValue ClearColor[] = { {m_ClearColor.r,m_ClearColor.g,m_ClearColor.b,m_ClearColor.a},{0.0f,0.0f} };

    VkRenderPassBeginInfo RenderPassBeginInfo{};
    RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    RenderPassBeginInfo.renderPass = m_RenderPass;
    RenderPassBeginInfo.renderArea.offset = { 0,0 };
    RenderPassBeginInfo.renderArea.extent = m_SwapChain->GetExtent();
    RenderPassBeginInfo.pClearValues = ClearColor;
    RenderPassBeginInfo.clearValueCount = 2;
    RenderPassBeginInfo.framebuffer = m_FrameBuffers[m_CurrentFrame].GetFrameBuffer(0);


    VkDeviceSize Offset{ 0 };
    vkCmdBeginRenderPass(m_CurrentCommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    for (int i = 0; i < m_DrawCommands.size(); i++) {

        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, m_VertexBuffers[i]->GetBuffer(), &Offset);

        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, *m_IndexBuffers[0]->GetBuffer(), Offset, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, m_DescriptorLayout.GetDescriptorSet(), 0, nullptr);

        vkCmdDrawIndexed(m_CurrentCommandBuffer, uint32_t(m_DrawCommands[i].VertexCount * 1.5f), 1, 0, 0, 0);

    }
    vkCmdEndRenderPass(m_CurrentCommandBuffer);

}
