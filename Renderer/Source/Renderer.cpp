#include "Renderer.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Device.h"
#include "InputSystem.h"
#include "Texture.h"
#include "Context.h"
#include "AssetManager.h"
Renderer::Renderer(RendererDesc desc, GLFWwindow* window, InputSystem* inputsystem,AssetManager* assetManager) {
    m_Window = window;
    m_ClearColor = desc.ClearColor;
    m_AssetManager = assetManager;
    //m_VertexBufferSize = 3*100;
   //Create vulkan instance
    InstanceDesc instancedesc{};
    instancedesc.ApiVersion = VK_API_VERSION_1_3;
    instancedesc.ValidationLayersEnabled = true;
    m_Instance = VulkanInstance::CreateInstance(instancedesc, &m_Messenger);

    CreateSurface(window, &m_Surface);

    m_PhysicalDevice = VulkanInstance::GetPhysicalDevice(m_Instance, m_Surface);
    m_SwapChainDetails = SwapChain::GetSwapChainCapabilities(m_PhysicalDevice, m_Surface);

    m_QueueFamilies = VulkanInstance::GetQueueFamilies(m_PhysicalDevice, m_Surface);
    m_Device = LogicalDevice::CreateLogicalDevice(m_PhysicalDevice, m_QueueFamilies);
    LogicalDevice::GetQueues(m_Device, m_QueueFamilies, &m_GraphicsQ, &m_PresentationQ);



    m_SwapChain = new SwapChain(m_Instance, m_PhysicalDevice, m_Device, m_Surface, m_QueueFamilies);
    m_SwapChainDetails = m_SwapChain->GetSwapChainCapabilities();

    m_SwapChain->CreateSwapChain();

    //One is reserved for white texture
    m_TextureIDByOrder.resize(m_TextureSlotCount-1);
    //Matrices
    m_UniformBuffers = new UniformBuffer[MAX_FRAME_DRAWS];
    for (int i = 0; i < MAX_FRAME_DRAWS; i++) {
        m_UniformBuffers[i].Init(m_Device, m_PhysicalDevice, sizeof(glm::mat4));


    }
    BufferDesc UniformBufferDesc{};
    UniformBufferDesc.Device = m_Device;
    UniformBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    UniformBufferDesc.Physdevice = m_PhysicalDevice;
    UniformBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
    UniformBufferDesc.Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    UniformBufferDesc.SizeBytes = sizeof(glm::mat4);

    m_UniformGUICameraBuffer = new Buffer(UniformBufferDesc);

  
    


    //m_DescriptorLayout[0].Init(m_Device,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,VK_SHADER_STAGE_VERTEX_BIT);
   

   // m_DescriptorLayout[1].Init(m_Device, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    Context context = new ContextData();
    context->PDevice = m_PhysicalDevice;
    context->Device = m_Device;
    context->GraphicsQueue = m_GraphicsQ;
    context->QueueFamil = m_QueueFamilies;

    m_DescriptorPool.AddDescriptorType(1, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
    m_DescriptorPool.AddDescriptorType(1, VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);


    m_DescriptorPool.CreatePool(context);

    DescriptorSetDescription CameraDescriptordesc{ context };
    CameraDescriptordesc.DescriptorCount = 1;
    CameraDescriptordesc.DescriptorPool = m_DescriptorPool.GetPool();
    CameraDescriptordesc.StageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    CameraDescriptordesc.Type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    m_DescriptorSetCamera.Init(CameraDescriptordesc);
    m_GUICameraDescriptor.Init(CameraDescriptordesc);

    DescriptorSetDescription TextureDescriptordesc{ context };
    TextureDescriptordesc.DescriptorCount = 4;
    TextureDescriptordesc.DescriptorPool = m_DescriptorPool.GetPool();
    TextureDescriptordesc.StageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    TextureDescriptordesc.Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
   // m_DescriptorSetTextures.Init(TextureDescriptordesc);

   

    m_RenderPass = Pipeline::CreateRenderPass(m_Device, m_SwapChain->GetFormat());

 

    VkFormat format = Core::ChooseBestFormat(m_PhysicalDevice, { VK_FORMAT_R32G32_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
    VkFormat DepthStencilFormat = Core::ChooseBestFormat(m_PhysicalDevice, { VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);


    m_ColorAttachments.resize(MAX_FRAME_DRAWS);
    m_DepthStencilAttachments.resize(MAX_FRAME_DRAWS);
    for (int i = 0; i < m_ColorAttachments.size(); i++) {

        m_ColorAttachments[i] = Image(m_PhysicalDevice, m_Device, format, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, m_SwapChain->GetExtent().width, m_SwapChain->GetExtent().height);
        m_DepthStencilAttachments[i] = Image(m_PhysicalDevice, m_Device, DepthStencilFormat, VK_SHARING_MODE_EXCLUSIVE, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, m_SwapChain->GetExtent().width, m_SwapChain->GetExtent().height,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_ASPECT_STENCIL_BIT);
    }
    CreateFrameBuffers();


    CreateCommandBuffers();
    CreateSamaphore();


    m_VertexCount = desc.VertexCountPerDrawCall;
    BufferDesc VertexBufferDesc{};
    VertexBufferDesc.SizeBytes = desc.VertexCountPerDrawCall * sizeof(Vertex);
    VertexBufferDesc.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VertexBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
    VertexBufferDesc.Physdevice = m_PhysicalDevice;
    VertexBufferDesc.Device = m_Device;
    VertexBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;


    m_VertexBufferGeometry.push_back(new Buffer(VertexBufferDesc));
    VertexBufferDesc.SizeBytes = m_VertexMaxCountGUI * sizeof(Vertex);
    m_VertexBufferGUI.push_back(new Buffer(VertexBufferDesc));

    m_VertexBufferOutlines.push_back(new Buffer(VertexBufferDesc));



    // VertexBufferDesc.SizeBytes = desc.VertexCountPerDrawCall * sizeof(Vertex)*20000;

     //Buffer* vertexdwadad = new Buffer(VertexBufferDesc);
    m_VerticesGUI = new Vertex[m_VertexMaxCountGUI];
    m_Vertices = new Vertex[m_VertexCount];
    m_VertexOutline = new Vertex[m_VertexOutineMaxCountPerDrawCall];

    BufferDesc StaggingBufferDesc{};
    StaggingBufferDesc.SizeBytes = desc.VertexCountPerDrawCall * sizeof(Vertex);
    StaggingBufferDesc.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    StaggingBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
    StaggingBufferDesc.Physdevice = m_PhysicalDevice;
    StaggingBufferDesc.Device = m_Device;
    StaggingBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    m_StaggingBufferGeometry.push_back(new Buffer(StaggingBufferDesc));
    StaggingBufferDesc.SizeBytes = m_VertexMaxCountGUI * sizeof(Vertex);
    m_StaggingBufferGUI.push_back(new Buffer(StaggingBufferDesc));

    m_StaggingBufferOutlines.push_back(new Buffer(StaggingBufferDesc));


    //temp

    m_Indices = new uint32_t[(uint32_t)(m_VertexCount * 1.5)];
    uint32_t offset{};
    for (int i = 0; i < m_VertexCount * 1.5; i += 6) {

        m_Indices[i] = offset;
        m_Indices[i + 1] = offset + 1;
        m_Indices[i + 2] = offset + 2;

        m_Indices[i + 3] = offset + 2;
        m_Indices[i + 4] = offset + 3;
        m_Indices[i + 5] = offset;
        offset += 4;
    }


    BufferDesc IndexBufferDesc{};
    IndexBufferDesc.SizeBytes = uint64_t(desc.VertexCountPerDrawCall * sizeof(uint32_t) * 1.5f);
    IndexBufferDesc.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    IndexBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
    IndexBufferDesc.Physdevice = m_PhysicalDevice;
    IndexBufferDesc.Device = m_Device;
    IndexBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;




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
    PickingImageBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;


    m_PickingImageBuffer = new Buffer(PickingImageBufferDesc);

    //TEMP code



    uint32_t* Pixels = new uint32_t[1];
   
  

    for (int i = 0; i < 1; i++) {
        Pixels[i] = 0xFFFFFFFF;
    }
    context->CommandPool = m_GraphicsPool.GetCommandPool();

    m_BlankWhiteTexture = new Texture(context, 1, 1,4, Pixels);



    //Initialize GUI renderer
    m_DescriptorSetCamera.WriteTo(0, *m_UniformBuffers[0].GetBuffer(), sizeof(glm::mat4));
    m_GUICameraDescriptor.WriteTo(0, *m_UniformGUICameraBuffer->GetBuffer(), sizeof(glm::mat4));

    if (desc.InitialCamera) 
     m_UniformBuffers->UploadToBuffer(m_Device, &desc.InitialCamera->GetViewProj(), sizeof(glm::mat4));

    
   // m_DescriptorSetTextures.WriteToTexture(0, WhiteTexture.GetImageView(), WhiteTexture.GetSampler());
  //  m_DescriptorSetTextures.WriteToTexture(1, WhiteTexture.GetImageView(), WhiteTexture.GetSampler());
   // m_DescriptorSetTextures.WriteToTexture(2, WhiteTexture.GetImageView(), WhiteTexture.GetSampler());
  //  m_DescriptorSetTextures.WriteToTexture(3, WhiteTexture.GetImageView(), WhiteTexture.GetSampler());

    m_Context = new ContextData();
    m_Context->Device = m_Device;
    m_Context->PDevice = m_PhysicalDevice;
    m_Context->QueueFamil = m_QueueFamilies;
    m_Context->GraphicsQueue = m_GraphicsQ;
    m_Context->CommandPool = m_GraphicsPool.GetCommandPool();


 }
 void Renderer::InitializePipeline(uint64_t MaxTextureCount)
 {
     uint32_t DescriptorPoolSize = (uint32_t)std::ceil((float)(MaxTextureCount*10) / (float)m_TextureSlotCount);
     for (uint64_t i = 0; i < DescriptorPoolSize; i++) {

     m_DescriptorPoolTextures.AddDescriptorType(m_TextureSlotCount, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
     m_DescriptorPoolTextures.CreatePool(m_Context);
     }

     DescriptorSetDescription DescriptorDesc{};
     DescriptorDesc.context = m_Context;
     DescriptorDesc.DescriptorCount = 4;
     DescriptorDesc.DescriptorPool = m_DescriptorPoolTextures.GetPool();
     DescriptorDesc.StageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
     DescriptorDesc.Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

     

     VkDescriptorSetLayout* descriptorsetlayout = new VkDescriptorSetLayout[3];


        uint32_t Offset{ 0 };
     for (uint32_t i = 0; i < uint32_t(DescriptorPoolSize*0.5f); i++) {
         m_DescriptorSetTextures.emplace_back();
         m_DescriptorSetTextures[i].Init(DescriptorDesc);



        for (uint32_t j = 0; j < m_TextureSlotCount; j++) {
         m_DescriptorSetTextures[i].WriteToTexture(j, m_BlankWhiteTexture->GetImageView(), m_BlankWhiteTexture->GetSampler());

        }
     }
     
        descriptorsetlayout[0] = m_DescriptorSetCamera.GetDescriptorLayout();
        descriptorsetlayout[1] = m_DescriptorSetTextures[0].GetDescriptorLayout();
        descriptorsetlayout[2] = m_GUICameraDescriptor.GetDescriptorLayout();


     m_PipelineLayout = Pipeline::CreatePipelineLayout(m_Device, descriptorsetlayout,3);
     
     PipelineDesc pipelineDesc{};
     pipelineDesc.RenderPass = m_RenderPass;
     pipelineDesc.PipelineLayout = m_PipelineLayout;
     pipelineDesc.VertexStageInputCount = 5;
     pipelineDesc.VertexInputStride = sizeof(Vertex);
     pipelineDesc.VertexStageInput = new VertexStageInputAttrib[pipelineDesc.VertexStageInputCount];
     pipelineDesc.VertexStageInput[0] = { VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex,Position),0,0 };
     pipelineDesc.VertexStageInput[1] = { VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(Vertex,Color),1,0 };
     pipelineDesc.VertexStageInput[2] = { VK_FORMAT_R32G32_UINT,offsetof(Vertex,ID),2,0 };
     pipelineDesc.VertexStageInput[3] = { VK_FORMAT_R32G32_SFLOAT,offsetof(Vertex,TexCoords),3,0 };
     pipelineDesc.VertexStageInput[4] = { VK_FORMAT_R32_UINT,offsetof(Vertex,TextureID),4,0 };



     pipelineDesc.Viewport = { 0,0,(float)m_SwapChain->GetExtent().width,(float)m_SwapChain->GetExtent().height,0.0f,1.0f };


     m_Pipeline = Pipeline::CreatePipeline(pipelineDesc, m_Device);
 }
    void Renderer::BeginFrame(Camera2D* camera){
               //RecordCommands(m_VertexCount,0);
       
            m_DrawCallCountGUI = 0;
            m_DrawCallCountGeometry =0;
            m_DrawCallCountOutlines = 0;

            m_Camera = *camera;
            m_CameraViewProj = m_Camera.GetViewProj();

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




            if (m_CurrentFrame ==1)
                vkCmdCopyImageToBuffer(m_CurrentCommandBuffer, m_ColorAttachments[0].GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, *m_PickingImageBuffer->GetBuffer(), 1, &copyregion);
            
          


    }

    void Renderer::BeginGUIFrame()
    {
        FlushGeometry();
      // m_CameraViewProj = glm::identity<glm::mat4>();
       m_GUIRendering = true;
    }


   
   
    void Renderer::CreateNewBufferForBatch(std::vector<Buffer*>& VertexBuffers, std::vector<Buffer*>& Stagging)
    {


        VertexBuffers.push_back(new Buffer(VertexBuffers[0]->GetBufferDesc()));
        Stagging.push_back(new Buffer(Stagging[0]->GetBufferDesc()));

      
        
    }
    void Renderer::FlushGUI()
    {
        uint32_t Index{ 1 };

        for (auto& it : m_Textures) {
           m_DescriptorSetTextures[m_DrawCallCountGUI+m_DrawCallCountGeometry].WriteToTexture(Index, it.second.texture->GetImageView(), it.second.texture->GetSampler());
            Index++;
        }


        m_DrawCommandsGUI.push_back({ m_VertexPointer,m_DrawCallCountGUI + m_DrawCallCountGeometry });

        if (m_DrawCallCountGUI == m_VertexBufferGUI.size())
            CreateNewBufferForBatch(m_VertexBufferGUI, m_StaggingBufferGUI);

        glm::mat4 Identity = glm::identity<glm::mat4>();
        m_UniformGUICameraBuffer->UploadToBuffer(m_Device, &Identity, sizeof(glm::mat4));

        m_StaggingBufferGUI[m_DrawCallCountGUI]->UploadToBuffer(m_Device, m_Vertices, sizeof(Vertex) * m_VertexPointer);

        VkBufferCopy region{};
        region.size = sizeof(Vertex) * m_VertexPointer;

        if (m_VertexPointer != 0)
        {
            vkCmdCopyBuffer(m_CurrentCommandBuffer, *m_StaggingBufferGUI[m_DrawCallCountGUI]->GetBuffer(), *m_VertexBufferGUI[m_DrawCallCountGUI]->GetBuffer(), 1, &region);
        }





        m_DrawCallCountGUI++;
        m_VertexPointer = 0;;
        m_Textures.clear();
    }
    void Renderer::FlushOutlines()
    {
   


        m_DrawCommandsOutlines.push_back({ m_VertexCountOutlines,0 });


        if (m_DrawCallCountOutlines == m_VertexBufferOutlines.size())
            CreateNewBufferForBatch(m_VertexBufferOutlines, m_StaggingBufferOutlines);

        m_UniformBuffers[0].UploadToBuffer(m_Device, &m_CameraViewProj, sizeof(glm::mat4));

        m_StaggingBufferOutlines[m_DrawCallCountOutlines]->UploadToBuffer(m_Device, m_VertexOutline, sizeof(Vertex) * m_VertexCountOutlines);


        VkBufferCopy region{};
        region.size = sizeof(Vertex) * m_VertexCountOutlines;

        if (m_VertexCountOutlines != 0)
        {
            vkCmdCopyBuffer(m_CurrentCommandBuffer, *m_StaggingBufferOutlines[m_DrawCallCountOutlines]->GetBuffer(), *m_VertexBufferOutlines[m_DrawCallCountOutlines]->GetBuffer(), 1, &region);
        }









        m_VertexCountOutlines = 0;
        m_DrawCallCountOutlines++;
    }
    
    void Renderer::FlushGeometry()
    {
        if (m_GUIRendering) {
            FlushGUI();
            return;
        }

        for (uint32_t i = 0; i < m_Textures.size(); i++) {
            Texture* texture = m_AssetManager->GetResource<Texture>(m_TextureIDByOrder[i]);
            if (texture)
                m_DescriptorSetTextures[m_DrawCallCountGeometry].WriteToTexture(i + 1, texture->GetImageView(), texture->GetSampler());
            else
                Core::Log(ErrorType::Error, "Texture id was invalid.");
        }


        m_DrawCommandsGeometry.push_back({ m_VertexPointer,m_DrawCallCountGUI + m_DrawCallCountGeometry });

        if (m_DrawCallCountGeometry == m_VertexBufferGeometry.size())
            CreateNewBufferForBatch(m_VertexBufferGeometry,m_StaggingBufferGeometry);

            m_UniformBuffers[0].UploadToBuffer(m_Device, &m_CameraViewProj, sizeof(glm::mat4));

            m_StaggingBufferGeometry[m_DrawCallCountGeometry]->UploadToBuffer(m_Device, m_Vertices, sizeof(Vertex) * m_VertexPointer);


        VkBufferCopy region{};
        region.size = sizeof(Vertex) * m_VertexPointer;

        if (m_VertexPointer != 0)
        {
            vkCmdCopyBuffer(m_CurrentCommandBuffer, *m_StaggingBufferGeometry[m_DrawCallCountGeometry]->GetBuffer(), *m_VertexBufferGeometry[m_DrawCallCountGeometry]->GetBuffer(), 1, &region);
        }

        







        m_VertexPointer = 0;
        m_DrawCallCountGeometry++;
        m_Textures.clear();
    }
 
 
    
    void Renderer::EndFrame(){
        FlushOutlines();
        FlushGUI();

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
       m_GUIRendering = false;
    }

    void Renderer::DrawQuad(Float3 Position, Float4 Color, Float2 Size, GUUID TextureHandle, uint64_t ID,uint32_t TextureIndex)
    {
        if (m_VertexPointer + 4 > m_VertexCount )
            FlushGeometry();
        if (TextureHandle != 0) {
            if (m_Textures.find(TextureHandle) == m_Textures.end()) {
                if (m_Textures.size() == m_TextureSlotCount - 1)
                    FlushGeometry();
                m_Textures[TextureHandle] = { m_AssetManager->GetResource<Texture>(TextureHandle) ,(uint32_t)m_Textures.size() + 1 };
                m_TextureIDByOrder[m_Textures.size() - 1] = TextureHandle;

            }
            TextureRenderingData texture = m_Textures[TextureHandle];
            Texture* texturetest = m_AssetManager->GetResource<Texture>(TextureHandle);
            m_Vertices[m_VertexPointer].TextureID = texture.Index;
            m_Vertices[m_VertexPointer + 1].TextureID = texture.Index;
            m_Vertices[m_VertexPointer + 2].TextureID = texture.Index;
            m_Vertices[m_VertexPointer + 3].TextureID = texture.Index;

            m_Vertices[m_VertexPointer].TexCoords = texture.texture->GetTextureCoords()[0];
            m_Vertices[m_VertexPointer + 1].TexCoords = texture.texture->GetTextureCoords()[1];
            m_Vertices[m_VertexPointer + 2].TexCoords = texture.texture->GetTextureCoords()[2];
            m_Vertices[m_VertexPointer + 3].TexCoords = texture.texture->GetTextureCoords()[3];
        }
        m_Vertices[m_VertexPointer].Position = { Position.x - Size.x,Position.y - Size.y };
        m_Vertices[m_VertexPointer + 1].Position = { Position.x - Size.x,Position.y + Size.y };
        m_Vertices[m_VertexPointer + 2].Position = { Position.x + Size.x,Position.y + Size.y };
        m_Vertices[m_VertexPointer + 3].Position = { Position.x + Size.x,Position.y - Size.y };

      


        m_Vertices[m_VertexPointer].Color = Color;
        m_Vertices[m_VertexPointer + 1].Color = Color;
        m_Vertices[m_VertexPointer + 2].Color = Color;
        m_Vertices[m_VertexPointer + 3].Color = Color;



        m_Vertices[m_VertexPointer].ID = ID;
        m_Vertices[m_VertexPointer + 1].ID = ID;
        m_Vertices[m_VertexPointer + 2].ID = ID;
        m_Vertices[m_VertexPointer + 3].ID = ID;

           
       


        m_VertexPointer += 4;


    }

  

    void Renderer::DrawQuad(Float3 Position, Float4 Color, Float2 Size, Animator Animation, uint64_t ID)
    {
        if (m_VertexPointer + 4 > m_VertexCount)
            FlushGeometry();
            if (m_Textures.find(Animation.GetCurrentTextureID()) == m_Textures.end()) {
                if (m_Textures.size() == m_TextureSlotCount - 1)
                    FlushGeometry();
                m_Textures[Animation.GetCurrentTextureID()] = { m_AssetManager->GetResource<Texture>(Animation.GetCurrentTextureID()) ,(uint32_t)m_Textures.size() + 1 };
                m_TextureIDByOrder[m_Textures.size() - 1] = Animation.GetCurrentTextureID();
            }
            TextureRenderingData texture = m_Textures[Animation.GetCurrentTextureID()];
            if (!texture.texture)
                return;
            m_Vertices[m_VertexPointer].TextureID = texture.Index;
            m_Vertices[m_VertexPointer + 1].TextureID = texture.Index;
            m_Vertices[m_VertexPointer + 2].TextureID = texture.Index;
            m_Vertices[m_VertexPointer + 3].TextureID = texture.Index;

            m_Vertices[m_VertexPointer].TexCoords = texture.texture->GetTextureCoords()[0];
            m_Vertices[m_VertexPointer + 1].TexCoords = texture.texture->GetTextureCoords()[1];
            m_Vertices[m_VertexPointer + 2].TexCoords = texture.texture->GetTextureCoords()[2];
            m_Vertices[m_VertexPointer + 3].TexCoords = texture.texture->GetTextureCoords()[3];


      
  
        m_Vertices[m_VertexPointer].Position = { Position.x - Size.x,Position.y - Size.y };
        m_Vertices[m_VertexPointer + 1].Position = { Position.x - Size.x,Position.y + Size.y };
        m_Vertices[m_VertexPointer + 2].Position = { Position.x + Size.x,Position.y + Size.y };
        m_Vertices[m_VertexPointer + 3].Position = { Position.x + Size.x,Position.y - Size.y };




        m_Vertices[m_VertexPointer].Color = Color;
        m_Vertices[m_VertexPointer + 1].Color = Color;
        m_Vertices[m_VertexPointer + 2].Color = Color;
        m_Vertices[m_VertexPointer + 3].Color = Color;



        m_Vertices[m_VertexPointer].ID = ID;
        m_Vertices[m_VertexPointer + 1].ID = ID;
        m_Vertices[m_VertexPointer + 2].ID = ID;
        m_Vertices[m_VertexPointer + 3].ID = ID;





        m_VertexPointer += 4;

    }

  

    void Renderer::DrawQuad(Float3 Position, Float4 Color, Float2 Size, uint64_t ID)
    {
        if (m_VertexPointer + 4 > m_VertexCount)
            FlushGeometry();

        m_Vertices[m_VertexPointer].Position = { Position.x - Size.x,Position.y - Size.y };
        m_Vertices[m_VertexPointer + 1].Position = { Position.x - Size.x,Position.y + Size.y };
        m_Vertices[m_VertexPointer + 2].Position = { Position.x + Size.x,Position.y + Size.y };
        m_Vertices[m_VertexPointer + 3].Position = { Position.x + Size.x,Position.y - Size.y };

        m_Vertices[m_VertexPointer].TextureID = 0;
        m_Vertices[m_VertexPointer + 1].TextureID = 0;
        m_Vertices[m_VertexPointer + 2].TextureID = 0;
        m_Vertices[m_VertexPointer + 3].TextureID = 0;


        m_Vertices[m_VertexPointer].Color = Color;
        m_Vertices[m_VertexPointer + 1].Color = Color;
        m_Vertices[m_VertexPointer + 2].Color = Color;
        m_Vertices[m_VertexPointer + 3].Color = Color;



        m_Vertices[m_VertexPointer].ID = ID;
        m_Vertices[m_VertexPointer + 1].ID = ID;
        m_Vertices[m_VertexPointer + 2].ID = ID;
        m_Vertices[m_VertexPointer + 3].ID = ID;

            m_Vertices[m_VertexPointer].TexCoords = { 0.0f,1.0f };
            m_Vertices[m_VertexPointer + 1].TexCoords = { 0.0f,0.0f };
            m_Vertices[m_VertexPointer + 2].TexCoords = { 1.0f,0.0f };
            m_Vertices[m_VertexPointer + 3].TexCoords = { 1.0f,1.0f };
     


        m_VertexPointer += 4;
    }

    void Renderer::SetCurrentFont(Texture* TextureFontAtlas)
    {
        m_FontTextureAtlas = TextureFontAtlas;
    }

    void Renderer::RenderText(const char* Message, Float2 Position, Float2 BoundingBox[4], float CharSize)
    {
        //
         float FixedPadding{0.00002f*CharSize};
        //temp
        Float4 Color{ 1.0f,1.0f,1.0f,1.0f };
        float Offset{};
        GUUID TextureHandle = Core::GetStringHash("FONTAtlas");
        float Space{ 0.06f };

        if (m_Textures.size() == m_TextureSlotCount - 1)
            FlushGeometry();
        m_Textures[TextureHandle] = { m_FontTextureAtlas ,(uint32_t)m_Textures.size() + 1 };
        m_TextureIDByOrder[m_Textures.size() - 1] = TextureHandle;

        //Do this for every letter
        for (uint32_t i = 0; i < strlen(Message); i++) {
            Float2 SubTextureSize{};
            Float2 Size{};
            int32_t LetterIndex = Message[i]-33;

            //edge cases
            //space letter index ==-1

            if (Message[i] == '\0')
                break;
            else if (Message[i] == ' ')
                Size.x = FixedPadding*2;
            if (m_VertexPointer + 4 > m_VertexCount)
                FlushGeometry();
         
            TextureRenderingData texture = m_Textures[TextureHandle];


            m_Vertices[m_VertexPointer].TextureID = texture.Index;
            m_Vertices[m_VertexPointer + 1].TextureID = texture.Index;
            m_Vertices[m_VertexPointer + 2].TextureID = texture.Index;
            m_Vertices[m_VertexPointer + 3].TextureID = texture.Index;

            if (LetterIndex != -1) {
                Size.x = texture.texture->GetSubTextureSize(LetterIndex).x/ texture.texture->GetWidth();
                Size.y = texture.texture->GetSubTextureSize(LetterIndex).y / texture.texture->GetHeight();

                m_Vertices[m_VertexPointer].TexCoords = texture.texture->GetSubTextureCoords(LetterIndex)[0];
                m_Vertices[m_VertexPointer + 1].TexCoords = texture.texture->GetSubTextureCoords(LetterIndex)[1];
                m_Vertices[m_VertexPointer + 2].TexCoords = texture.texture->GetSubTextureCoords(LetterIndex)[2];
                m_Vertices[m_VertexPointer + 3].TexCoords = texture.texture->GetSubTextureCoords(LetterIndex)[3];
            }
            else {
                m_Vertices[m_VertexPointer].TexCoords = { 0.0f,0.0f };
                m_Vertices[m_VertexPointer + 1].TexCoords = { 0.0f,0.0f };
                m_Vertices[m_VertexPointer + 2].TexCoords = { 0.0f,0.0f };
                m_Vertices[m_VertexPointer + 3].TexCoords = { 0.0f,0.0f };
            }
            m_Vertices[m_VertexPointer].Position = { BoundingBox[0].x+ Offset,BoundingBox[0].y,0.0f };
            m_Vertices[m_VertexPointer + 1].Position = { BoundingBox[0].x+ Offset,BoundingBox[0].y+Size.y,0.0f };
            m_Vertices[m_VertexPointer + 2].Position = { BoundingBox[0].x+ Offset + Size.x,BoundingBox[0].y + Size.y,0.0f };
            m_Vertices[m_VertexPointer + 3].Position = { BoundingBox[0].x+ Offset + Size.x,BoundingBox[0].y,0.0f};




            m_Vertices[m_VertexPointer].Color = Color;
            m_Vertices[m_VertexPointer + 1].Color = Color;
            m_Vertices[m_VertexPointer + 2].Color = Color;
            m_Vertices[m_VertexPointer + 3].Color = Color;



            m_Vertices[m_VertexPointer].ID = 0;
            m_Vertices[m_VertexPointer + 1].ID = 0;
            m_Vertices[m_VertexPointer + 2].ID = 0;
            m_Vertices[m_VertexPointer + 3].ID = 0;





            m_VertexPointer += 4;
            Offset += Size.x+ FixedPadding;
        }
    }


    void Renderer::DrawOutline(Float3 Position, Float2 Size,Float4 Color,float OutlineWidth)
    {

        if (m_VertexCountOutlines + 4 > m_VertexOutineMaxCountPerDrawCall)
            FlushOutlines();
        m_VertexOutline[m_VertexCountOutlines].ID = 0;
        m_VertexOutline[m_VertexCountOutlines+1].ID = 0;
        m_VertexOutline[m_VertexCountOutlines+2].ID = 0;
        m_VertexOutline[m_VertexCountOutlines+3].ID = 0;

       // m_VertexOutline[m_VertexCountOutlines].Position = { Position.x - Size.x,Position.y - Size.y };
       // m_VertexOutline[m_VertexCountOutlines + 1].Position = { Position.x - Size.x,Position.y + Size.y };
       // m_VertexOutline[m_VertexCountOutlines + 2].Position = { Position.x + Size.x,Position.y + Size.y };
       // m_VertexOutline[m_VertexCountOutlines + 3].Position = { Position.x + Size.x,Position.y - Size.y };

       m_VertexOutline[m_VertexCountOutlines].Position = {Position.x-Size.x-OutlineWidth,Position.y-Size.y-OutlineWidth };
       m_VertexOutline[m_VertexCountOutlines + 1].Position = {Position.x-Size.x-OutlineWidth,Position.y+Size.y+OutlineWidth};
       m_VertexOutline[m_VertexCountOutlines + 2].Position = {Position.x+Size.x+OutlineWidth,Position.y+Size.y+OutlineWidth};
       m_VertexOutline[m_VertexCountOutlines + 3].Position = {Position.x+Size.x+OutlineWidth,Position.y-Size.y-OutlineWidth};

        m_VertexOutline[m_VertexCountOutlines].TextureID = 0;
        m_VertexOutline[m_VertexCountOutlines + 1].TextureID = 0;
        m_VertexOutline[m_VertexCountOutlines + 2].TextureID = 0;
        m_VertexOutline[m_VertexCountOutlines + 3].TextureID = 0;

        m_VertexOutline[m_VertexCountOutlines].Color = Color;
        m_VertexOutline[m_VertexCountOutlines + 1].Color = Color;
        m_VertexOutline[m_VertexCountOutlines + 2].Color = Color;
        m_VertexOutline[m_VertexCountOutlines + 3].Color = Color;



        m_VertexOutline[m_VertexCountOutlines].TexCoords = { 0.0f,1.0f };
        m_VertexOutline[m_VertexCountOutlines + 1].TexCoords = { 0.0f,0.0f };
        m_VertexOutline[m_VertexCountOutlines + 2].TexCoords = { 1.0f,0.0f };
        m_VertexOutline[m_VertexCountOutlines + 3].TexCoords = { 1.0f,1.0f };


        m_VertexCountOutlines += 4;
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
        Image images[]= {*m_SwapChain->GetSwapChainImage(i),m_ColorAttachments[i],m_DepthStencilAttachments[i]};
                m_FrameBuffers[i].Init(m_Device,m_SwapChain,m_RenderPass,images,3);
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

        Core::Log(ErrorType::Info,"Draw call count",m_DrawCallCountGeometry+m_DrawCallCountGUI+m_DrawCallCountOutlines);
    }

    Texture* Renderer::LoadTexture(std::string Path)
    {
        Context context = new ContextData();
        context->Device = m_Device;
        context->PDevice = m_PhysicalDevice;
        context->GraphicsQueue = m_GraphicsQ;
        context->QueueFamil = m_QueueFamilies;
        context->CommandPool = m_GraphicsPool.GetCommandPool();
        Texture*texture = new Texture(context, Path);
        return texture;
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
    m_DrawCommandsGeometry.resize(0);
    m_DrawCommandsOutlines.resize(0);
    m_DrawCommandsGUI.resize(0);
}

void Renderer::DrawBatch()
{
    VkClearValue ClearColor[] = { {m_ClearColor.r,m_ClearColor.g,m_ClearColor.b,m_ClearColor.a},{0.0f,0.0f},{0.0f,0xFF} };

    VkRenderPassBeginInfo RenderPassBeginInfo{};
    RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    RenderPassBeginInfo.renderPass = m_RenderPass;
    RenderPassBeginInfo.renderArea.offset = { 0,0 };
    RenderPassBeginInfo.renderArea.extent = m_SwapChain->GetExtent();
    RenderPassBeginInfo.pClearValues = ClearColor;          
    RenderPassBeginInfo.clearValueCount = 3;
    RenderPassBeginInfo.framebuffer = m_FrameBuffers[m_CurrentFrame].GetFrameBuffer(0);


    VkDeviceSize Offset{ 0 };
    vkCmdBeginRenderPass(m_CurrentCommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdSetStencilTestEnable(m_CurrentCommandBuffer, VK_TRUE);

    vkCmdSetStencilWriteMask(m_CurrentCommandBuffer, VK_STENCIL_FACE_FRONT_BIT, 0xFF);
    vkCmdSetStencilOp(m_CurrentCommandBuffer, VK_STENCIL_FACE_FRONT_BIT, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_REPLACE, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS);
    vkCmdSetStencilReference(m_CurrentCommandBuffer, VK_STENCIL_FACE_FRONT_BIT, 1);
    vkCmdSetStencilCompareMask(m_CurrentCommandBuffer, VK_STENCIL_FACE_FRONT_BIT, 1);
    for (int i = 0; i < m_DrawCommandsGeometry.size(); i++) {
      
        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, m_VertexBufferGeometry[i]->GetBuffer(), &Offset);

        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, *m_IndexBuffers[0]->GetBuffer(), Offset, VK_INDEX_TYPE_UINT32);

        VkDescriptorSet DescriptorSets[2];
        DescriptorSets[1] = m_DescriptorSetTextures[m_DrawCommandsGeometry[i].DescriptorSetTextureIndex].GetDescriptorSet();
 

       
            DescriptorSets[0] = m_DescriptorSetCamera.GetDescriptorSet();


        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 2, DescriptorSets, 0, nullptr);


        vkCmdDrawIndexed(m_CurrentCommandBuffer, uint32_t(m_DrawCommandsGeometry[i].VertexCount * 1.5f), 1, 0, 0, 0);

    }

    vkCmdSetStencilWriteMask(m_CurrentCommandBuffer, VK_STENCIL_FACE_FRONT_BIT, 0x00);
    vkCmdSetStencilOp(m_CurrentCommandBuffer, VK_STENCIL_FACE_FRONT_BIT, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_REPLACE, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_NOT_EQUAL);
    //vkCmdSetStencilTestEnable(m_CurrentCommandBuffer, VK_FALSE);


    for (uint32_t i = 0;i< m_DrawCommandsOutlines.size() ; i++) {
       
        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, m_VertexBufferOutlines[i]->GetBuffer(), &Offset);

        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, *m_IndexBuffers[0]->GetBuffer(), Offset, VK_INDEX_TYPE_UINT32);

        VkDescriptorSet DescriptorSets[2];
        DescriptorSets[1] = m_DescriptorSetTextures[0].GetDescriptorSet();

       
            DescriptorSets[0] = m_DescriptorSetCamera.GetDescriptorSet();


      


        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 2, DescriptorSets, 0, nullptr);


        vkCmdDrawIndexed(m_CurrentCommandBuffer, uint32_t(m_DrawCommandsOutlines[i].VertexCount * 1.5f), 1, 0, 0, 0);
    }
 

    vkCmdSetStencilWriteMask(m_CurrentCommandBuffer, VK_STENCIL_FACE_FRONT_BIT, 0xFF);
    vkCmdSetStencilOp(m_CurrentCommandBuffer, VK_STENCIL_FACE_FRONT_BIT, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_REPLACE, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS);

    for (uint32_t i = 0; i < m_DrawCommandsGUI.size(); i++) {
        

        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, m_VertexBufferGUI[i]->GetBuffer(), &Offset);

        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, *m_IndexBuffers[0]->GetBuffer(), Offset, VK_INDEX_TYPE_UINT32);

        VkDescriptorSet DescriptorSets[2];
        DescriptorSets[1] = m_DescriptorSetTextures[m_DrawCommandsGUI[i].DescriptorSetTextureIndex].GetDescriptorSet();


        DescriptorSets[0] = m_GUICameraDescriptor.GetDescriptorSet();





        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 2, DescriptorSets, 0, nullptr);


        vkCmdDrawIndexed(m_CurrentCommandBuffer, uint32_t(m_DrawCommandsGUI[i].VertexCount * 1.5f), 1, 0, 0, 0);
    }
   
    vkCmdEndRenderPass(m_CurrentCommandBuffer);

}

void Renderer::DrawGUIBatch()
{
    /*
    VkDeviceSize Offset{ 0 };
    for (int i = 0; i < m_DrawGUICommands.size(); i++) {

        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, m_VertexGUIBuffers[i]->GetBuffer(), &Offset);

        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, *m_IndexBuffers[0]->GetBuffer(), Offset, VK_INDEX_TYPE_UINT32);

        VkDescriptorSet DescriptorSets[] = { m_GUICameraDescriptor.GetDescriptorSet(),m_DescriptorSetTexturesGUI[i].GetDescriptorSet()};

        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 2, DescriptorSets, 0, nullptr);


        vkCmdDrawIndexed(m_CurrentCommandBuffer, uint32_t(m_DrawGUICommands[i].VertexCount * 1.5f), 1, 0, 0, 0);

    }
    */
}
