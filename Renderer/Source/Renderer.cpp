#include "Renderer.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Device.h"
#include "InputSystem.h"
#include "Texture.h"
#include "Context.h"
#include "AssetManager.h"
#include "FontSystem.h"
#include "GUI.h"
#include "CommandBuffer.h"


#include "Debug.h"


#include "glslang/Public/ShaderLang.h"


Renderer::Renderer(RendererDesc desc, GLFWwindow* window, InputSystem* inputsystem,AssetManager* assetManager) {
    
    m_RendererDesc = desc;
    m_RendererDesc.Viewport.minDepth = 1.0f;
    m_RendererDesc.Viewport.maxDepth = 0.0f;

    m_Window = window;
    m_ClearColor = desc.ClearColor;
    m_AssetManager = assetManager;
    //m_VertexBufferSize = 3*100;
   //Create vulkan instance

    InstanceDesc instancedesc{};
    instancedesc.ApiVersion = VK_API_VERSION_1_4;
    instancedesc.ValidationLayersEnabled = true;
    m_Instance = VulkanInstance::CreateInstance(instancedesc, &m_Messenger);

    CreateSurface(window, &m_Surface);

    VulkanInstance::GetPhysicalDevice(m_Instance, m_Surface,&m_PhysicalDevice);

    m_SwapChainDetails = SwapChain::GetSwapChainCapabilities(m_PhysicalDevice, m_Surface);

    m_QueueFamilies = VulkanInstance::GetQueueFamilies(m_PhysicalDevice, m_Surface);

   m_Device = LogicalDevice::CreateLogicalDevice(m_PhysicalDevice, m_QueueFamilies);

    LogicalDevice::GetQueues(m_Device, m_QueueFamilies, &m_GraphicsQ, &m_PresentationQ);

       Context context = new ContextData();
    context->PDevice = m_PhysicalDevice;
    context->Device = m_Device;
    context->GraphicsQueue = m_GraphicsQ;
    context->QueueFamil = m_QueueFamilies;
    
    m_Context = context;
    m_SwapChain = new SwapChain(m_Window,m_Instance,m_Context, m_Surface);
    m_SwapChainDetails = m_SwapChain->GetSwapChainCapabilities();
    m_SwapChain->CreateSwapChain(MAX_FRAME_DRAWS);

    //Allocation 
    m_Particles = new InstanceParticleData[m_MaxParticleCount];
   
    BufferDesc UniformBufferDesc{};
    UniformBufferDesc.Device = m_Device;
    UniformBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    UniformBufferDesc.Physdevice = m_PhysicalDevice;
    UniformBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
    UniformBufferDesc.Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    UniformBufferDesc.SizeBytes = sizeof(UniformCameraBufferData);  
    for(uint32_t i =0;i < m_UniformBuffer.size();i++)
         m_UniformBuffer[i] = new Buffer(UniformBufferDesc);
  
    CreateCommandBuffers();
    context->CommandPool =m_GraphicsPool.GetCommandPool();
    m_Context->CommandPool = m_GraphicsPool.GetCommandPool();

    uint32_t* Pixels = new uint32_t[1];

       Pixels[0] = 0xFFFFFFFF;
    TextureCreateInfo whiteTextureCreateInfo{};
    whiteTextureCreateInfo.Width =1;
    whiteTextureCreateInfo.Height=1;
    whiteTextureCreateInfo.Pixels = Pixels;
    whiteTextureCreateInfo.Format = VK_FORMAT_R8G8B8A8_UNORM;
    whiteTextureCreateInfo.ImageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;

    m_BlankWhiteTexture = new Texture(context, whiteTextureCreateInfo,TextureType::Texture);
    delete[] Pixels;
    
    CreateDescriptorSets();
    m_RenderPass = Pipeline::CreateRenderPass(m_Device, m_SwapChain->GetFormat());

 

    VkFormat format = Core::ChooseBestFormat(m_PhysicalDevice, { VK_FORMAT_R32G32_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
    VkFormat DepthStencilFormat = Core::ChooseBestFormat(m_PhysicalDevice, { VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    TextureCreateInfo colorAttachTextureInfo{};
    colorAttachTextureInfo.Format =format;
    colorAttachTextureInfo.Width = m_SwapChain->GetExtent().width;
    colorAttachTextureInfo.Height = m_SwapChain->GetExtent().height;
    colorAttachTextureInfo.ImageTilling =  VK_IMAGE_TILING_OPTIMAL;
    colorAttachTextureInfo.ImageUsageFlags = VkImageUsageFlagBits(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    colorAttachTextureInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    colorAttachTextureInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;


    m_ColorAttachments.resize(MAX_FRAME_DRAWS);
    m_DepthStencilAttachments.resize(MAX_FRAME_DRAWS);
    for (int i = 0; i < m_ColorAttachments.size(); i++) {
        m_ColorAttachments[i] =new Texture(m_Context,colorAttachTextureInfo,TextureType::ColorAttachment);
    }
    //change creat info for depth buffer
    colorAttachTextureInfo.ImageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    colorAttachTextureInfo.ImageUsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    colorAttachTextureInfo.Format = DepthStencilFormat;

    for(uint32_t i=0;i < m_DepthStencilAttachments.size();i++)
        m_DepthStencilAttachments[i] =new Texture(m_Context,colorAttachTextureInfo,TextureType::DepthStencilAttachment);

    CreateFrameBuffers();


    CreateSamaphore();


    m_VertexCount = desc.VertexCountPerDrawCall;
    m_VertexMaxCountGUI = desc.VertexCountPerDrawCall;

    BufferDesc VertexBufferDesc{};
    VertexBufferDesc.SizeBytes = desc.VertexCountPerDrawCall * sizeof(Vertex);
    VertexBufferDesc.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VertexBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
    VertexBufferDesc.Physdevice = m_PhysicalDevice;
    VertexBufferDesc.Device = m_Device;
    VertexBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;


    m_VertexBufferGeometry.push_back(new Buffer(VertexBufferDesc));
    VertexBufferDesc.SizeBytes = m_VertexMaxCountGUI * sizeof(Vertex);
    m_VertexBufferGUI.push_back( new Buffer(VertexBufferDesc));

    m_VertexBufferOutlines.push_back(new Buffer(VertexBufferDesc));



    // VertexBufferDesc.SizeBytes = desc.VertexCountPerDrawCall * sizeof(Vertex)*20000;

     //Buffer* vertexdwadad = new Buffer(VertexBufferDesc);
    m_VerticesGUI = new Vertex[m_VertexMaxCountGUI];
    m_VerticesQuad = new Vertex[m_VertexCount];
    m_Vertices = new Vertex[m_VertexCount];

    BufferDesc StaggingBufferDesc{};
    StaggingBufferDesc.SizeBytes = desc.VertexCountPerDrawCall * sizeof(Vertex);
    StaggingBufferDesc.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    StaggingBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
    StaggingBufferDesc.Physdevice = m_PhysicalDevice;
    StaggingBufferDesc.Device = m_Device;
    StaggingBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    m_StaggingBufferGeometry.push_back(new Buffer(StaggingBufferDesc));
    StaggingBufferDesc.SizeBytes = m_VertexMaxCountGUI * sizeof(Vertex);
    m_StaggingBufferGUI.push_back( new Buffer(StaggingBufferDesc));

    m_StaggingBufferOutlines.push_back( new Buffer(StaggingBufferDesc));


    //temp

    m_Indices = new uint32_t[(uint32_t)(m_VertexCount * 1.5)];
    uint32_t offset{};
    for (int i = 0; i < m_VertexCount * 1.5; i += 6) {

        m_Indices[i] = offset+1;
        m_Indices[i + 1] = offset +2;
        m_Indices[i + 2] = offset + 3;

        m_Indices[i + 3] = offset +3;
        m_Indices[i + 4] = offset ;
        m_Indices[i + 5] = offset+1 ;
        offset += 4;
    }


    BufferDesc IndexBufferDesc{};
    IndexBufferDesc.SizeBytes = uint64_t(desc.VertexCountPerDrawCall * sizeof(uint32_t) * 1.5f);
    IndexBufferDesc.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    IndexBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
    IndexBufferDesc.Physdevice = m_PhysicalDevice;
    IndexBufferDesc.Device = m_Device;
    IndexBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;




    m_IndexBuffersQuad = new Buffer(IndexBufferDesc);
    m_IndexBufferVertices =new Buffer(IndexBufferDesc);

    m_IndexBuffersQuad->UploadToBuffer(m_Device, m_Indices, 0);
    for (int i = 0; i < m_VertexCount * 1.5; i++) {
        m_Indices[i] = i;
     }
    m_IndexBufferVertices->UploadToBuffer(m_Device,m_Indices,0);
    delete[] m_Indices;

    //VkFormat format2 = Core::ChooseBestFormat(m_PhysicalDevice,{ VK_FORMAT_R32G32_UINT },VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

// m_FrameImageIndexed = new Image(m_PhysicalDevice,m_Device,format2,VK_SHARING_MODE_EXCLUSIVE,VK_IMAGE_USAGE_STORAGE_BIT,  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT           ,VK_IMAGE_TILING_OPTIMAL,m_SwapChain->GetExtent().width,m_SwapChain->GetExtent().height);

    BufferDesc PickingImageBufferDesc{};
    PickingImageBufferDesc.SizeBytes = m_ColorAttachments[0]->GetByteSize();
    PickingImageBufferDesc.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    PickingImageBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
    PickingImageBufferDesc.Physdevice = m_PhysicalDevice;
    PickingImageBufferDesc.Device = m_Device;
    PickingImageBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT;


    m_PickingImageBuffer = new Buffer(PickingImageBufferDesc);

 

   
    context->CommandPool = m_GraphicsPool.GetCommandPool();
    //Init CameraUniformBuffer and descriptors.
    m_UniformCameraData.GeometryCamera = glm::identity<glm::mat4>();
    m_UniformCameraData.GUICamera = glm::identity<glm::mat4>();

    for(uint32_t i=0;i < m_UniformBuffer.size();i++){
        m_UniformBuffer[i]->UploadToBuffer(m_Device,&m_UniformCameraData,sizeof(UniformCameraBufferData));
        m_DescriptorSetCamera[i].WriteTo(0,1,*m_UniformBuffer[i]->GetBuffer(),sizeof(UniformCameraBufferData));

    }

    //Make swapchain image layout present
    VkCommandBuffer commandBuffer =CommandBuffer::StartSingleUseCommandBuffer(m_Context,m_Context->CommandPool);
    for(uint32_t i=0;i < m_SwapChain->GetSwapChainImageCount();i++){
        m_SwapChain->GetSwapChainImage(i)->TrasitionFormat(false,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,commandBuffer);
    }
    CommandBuffer::EndSingleUseCommandBuffer(m_Context,m_Context->CommandPool,commandBuffer);

 }
    void Renderer::OnWindowResize(uint32_t width,uint32_t height){
        m_ResizeWindow=true;
        m_NewWindowSize ={(float)width,(float)height};
    }
void Renderer::DrawParticle(const Float2& pos,const Float4 color,const Float2 size,GUUID textureID){

    m_Particles[m_CurrentParticleIndex].ParticlePos = {pos.x-size.x,pos.y-size.y,0.0};
    m_Particles[m_CurrentParticleIndex+1].ParticlePos = {pos.x-size.x,pos.y+size.y,0.0};
    m_Particles[m_CurrentParticleIndex+2].ParticlePos= {pos.x+size.x,pos.y+size.y,0.0};
    m_Particles[m_CurrentParticleIndex+3].ParticlePos = {pos.x+size.x,pos.y-size.y,0.0};

    m_Particles[m_CurrentParticleIndex].Color = color;
    m_Particles[m_CurrentParticleIndex+1].Color = color;
    m_Particles[m_CurrentParticleIndex+2].Color = color;
    m_Particles[m_CurrentParticleIndex+3].Color = color;

    m_CurrentParticleIndex+=4;
}
void Renderer::DrawVertices(Vertex* vertices,uint32_t vertexCount,GUUID textureID){
    if(!vertices)
        return;
    if (m_VertexPointerQuad + vertexCount > m_VertexCount )
        FlushGeometry();
    if(textureID != 0){
    auto textures = m_Textures[m_CurrentFrame];
    auto index = textures.find(textureID);
    if(index == textures.end()){

          if(textures.size() >= m_TextureSlotCount)
                FlushGeometry();    
            textures[textureID] = {m_AssetManager->GetAsset<Texture>(textureID),m_CurrentTextureDescriptorSetOffset};
            m_CurrentTextureDescriptorSetOffset++;
    }
    }
    
    memcpy(&m_Vertices[m_VertexPointerQuad],vertices,sizeof(Vertex)*vertexCount);   

    m_VertexPointerQuad+=vertexCount;
}
 void Renderer::InitializePipeline(uint64_t MaxTextureCount)
 {
    //reserve size for queued shaders
    m_QueuedShaders.reserve(20);

    std::array<VkDescriptorSetLayout,2> descriptorLayout;
    descriptorLayout[0]= m_DescriptorSetCamera[0].GetDescriptorLayout();
    descriptorLayout[1]= m_DescriptorSetTextures[0].GetDescriptorLayout();


     m_PipelineLayout = Pipeline::CreatePipelineLayout(m_Device, descriptorLayout.data(),descriptorLayout.size());
     
     m_PipelineDesc.RenderPass = m_RenderPass;
     m_PipelineDesc.PipelineLayout = m_PipelineLayout;
     m_PipelineDesc.VertexStageInputCount = 5;
     m_PipelineDesc.VertexInputStride = sizeof(Vertex);
     m_PipelineDesc.VertexStageInput = new VertexStageInputAttrib[m_PipelineDesc.VertexStageInputCount];
     m_PipelineDesc.VertexStageInput[0] = { VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex,Position),0,0 };
     m_PipelineDesc.VertexStageInput[1] = { VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(Vertex,Color),1,0 };
     m_PipelineDesc.VertexStageInput[2] = { VK_FORMAT_R32G32_UINT,offsetof(Vertex,ID),2,0 };
     m_PipelineDesc.VertexStageInput[3] = { VK_FORMAT_R32G32_SFLOAT,offsetof(Vertex,TexCoords),3,0 };
     m_PipelineDesc.VertexStageInput[4] = { VK_FORMAT_R32_UINT,offsetof(Vertex,TextureID),4,0 };


     m_PipelineDesc.Viewport.x =0;
     m_PipelineDesc.Viewport.y =0;

     m_PipelineDesc.Viewport.width =m_SwapChain->GetExtent().width;
     m_PipelineDesc.Viewport.height =m_SwapChain->GetExtent().height;
    
    

     
     m_PipelineDesc.Viewport.minDepth =0.0f;
     m_PipelineDesc.Viewport.maxDepth =0.0f;

    std::string defaultShaderPaths[] = { {"Shaders/DefaultPixel"},{"Shaders/DefaultVertex"}};


    for(uint32_t i=0;i < ARRAYSIZE(defaultShaderPaths);i++){
        auto shaderAsset = m_AssetManager->GetAsset<Shader>(defaultShaderPaths[i]);
        if(!shaderAsset){

            Core::Log(ErrorType::FatalError,"Default shader are non existent or provided with bad path");
            //TODO dispatch event to shutdown application can't continue forwards.
        }
        m_CurrentlyLoadedShaders.push_back(shaderAsset);

    }

    m_PipelineDesc.ShaderCount =m_CurrentlyLoadedShaders.size();
    m_PipelineDesc.ShaderModules = new VkShaderModule[m_CurrentlyLoadedShaders.size()];
    m_PipelineDesc.ShaderStages = new VkShaderStageFlagBits[m_CurrentlyLoadedShaders.size()];
    for(uint32_t i=0;i < m_CurrentlyLoadedShaders.size();i++){
        m_PipelineDesc.ShaderModules[i] = m_CurrentlyLoadedShaders[i].GetData()->GetShaderModule();
        m_PipelineDesc.ShaderStages[i] = m_CurrentlyLoadedShaders[i].GetData()->GetShaderStage();
    }


    ReCreatePipeline(m_PipelineDesc);
    CreateParticlePipeline();

    m_LoadedShaderPaths.clear();
    delete[] m_PipelineDesc.ShaderStages;
    delete[] m_PipelineDesc.ShaderModules;
 }
    void Renderer::ResizeWindow(){
            vkDeviceWaitIdle(m_Device);
            vkWaitForFences(m_Device,1,&m_DrawFences[m_CurrentFrame],false,1000*1000*1000);
                for(uint32_t i=0;i < m_SwapChain->GetSwapChainImageCount();i++){
            }

            m_Camera.SetViewportSize({(float)m_NewWindowSize.x,(float)m_NewWindowSize.y});

                m_FrameBuffers.clear();

            m_SwapChain->DestroyImageViews();
            vkDestroySwapchainKHR(m_Device,m_SwapChain->GetSwapChain(),nullptr);
            m_SwapChain->CreateSwapChain(MAX_FRAME_DRAWS);


            ReCreateFrameBuffers();
            m_RendererDesc.Viewport.width = m_SwapChain->GetExtent().width;
            m_RendererDesc.Viewport.height = m_SwapChain->GetExtent().height;
            
            
           
            InitRenderDesc(m_RendererDesc);

            delete m_PickingImageBuffer;
            CreatePickingImage();

            
            for(uint32_t i=0 ;i < m_ImageAvailS.size();i++){

                vkDestroySemaphore(m_Device,m_ImageAvailS[i],nullptr);
                vkDestroySemaphore(m_Device,m_RenderFinishedS[i],nullptr);
                vkDestroyFence(m_Device,m_DrawFences[i],nullptr);
            }
        
            CreateSamaphore();
           


            vkResetCommandPool(m_Device,m_GraphicsPool.GetCommandPool(),VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
           VkCommandBuffer singleCommandBuffer= CommandBuffer::StartSingleUseCommandBuffer(m_Context,m_GraphicsPool.GetCommandPool());
            for(uint32_t i=0;i < m_SwapChain->GetSwapChainImageCount();i++){
                m_SwapChain->TransitionLayout(m_SwapChain->GetSwapChainImage(i)->GetImage(),VK_IMAGE_LAYOUT_UNDEFINED,singleCommandBuffer);
                m_SwapChain->TransitionLayout(m_SwapChain->GetSwapChainImage(i)->GetImage(),VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,singleCommandBuffer);
            }
            

            CommandBuffer::EndSingleUseCommandBuffer(m_Context,m_GraphicsPool.GetCommandPool(),singleCommandBuffer);

    }
    void Renderer::ResetFrameData(){
            m_DrawCallCountGUI = 0;
            m_DrawCallCountGeometry =0;
            m_DrawCallCountOutlines = 0;
            m_VertexCountRemaining = m_VertexCount;
            m_CurrentVertexBufferQuadIndex = 0;
            m_VertexBufferOffset = 0;
            m_VertexCountPerDrawCall = 0;
            m_VertexBufferOffsetGUI =0;
            m_VertexGUIRemaining = m_VertexMaxCountGUI;
            m_CurrentVertexBufferIndexGUI =0;

            m_CurrentVertexBufferIndex = 1;

            m_VertexCount = m_RendererDesc.VertexCountPerDrawCall;

            //Swap back to normal
            Vertex* temp = m_VerticesQuad;
            m_VerticesQuad = m_VerticesGUI;
            m_VerticesGUI = temp;

    }
    void Renderer::BeginFrame(Camera2D* camera,float deltaTime){    
    
        ResetFrameData();
        m_DeltaTime = deltaTime;
        
        m_CurrentCommandBuffer = m_CommandBuffers[m_CurrentFrame];
        m_Context->TransferCommandBuffer = m_TransferCommandBuffers[m_CurrentFrame];
        m_Context->CurrentCommandBuffer = m_CurrentCommandBuffer;
        m_Context->m_TempBuffers = &m_TempBuffers[m_CurrentFrame];
        
        if(m_ResizeWindow){
            ResizeWindow();
            m_ResizeWindow=false;
        }
        //recreate pipeline if needed
        if(m_RendererDesc.Rendermode != m_RendererDescNext.Rendermode){
            InitRenderDesc(m_RendererDescNext);
        }



            m_Camera = *camera;

              m_UniformCameraData.GeometryCamera = m_Camera.GetViewProj();
              m_UniformCameraData.GUICamera = glm::identity<glm::mat4>();
            m_UniformBuffer[m_CurrentFrame]->UploadToBuffer(m_Device, &m_UniformCameraData, sizeof(UniformCameraBufferData));

            VkSwapchainKHR swapchain = m_SwapChain->GetSwapChain();
        

             vkWaitForFences(m_Device,1,&m_DrawFences[m_CurrentFrame],true,std::numeric_limits<uint64_t>::max());
             vkResetFences(m_Device,1,&m_DrawFences[m_CurrentFrame]);

             //vkResetFences(m_Device,1,&m_ImageFreeF[m_CurrentFrame]);
        m_Textures[m_CurrentFrame].clear();
        m_Textures[m_CurrentFrame].rehash(0);

       m_AcquireImageResult =  vkAcquireNextImageKHR(m_Device,m_SwapChain->GetSwapChain(),1000000000,m_ImageAvailS[m_CurrentFrame],nullptr,&m_ImageIndex);
       if(m_AcquireImageResult == VK_ERROR_OUT_OF_DATE_KHR){
            ResizeWindow();
            return;
       }
       else if(m_AcquireImageResult != VK_SUCCESS){
           Core::Log(ErrorType::Error,"Failed to acquire image ",(int)m_AcquireImageResult);
            return;
       }


        m_DescriptorSetCamera[m_CurrentFrame].WriteTo(0,1,*m_UniformBuffer[m_CurrentFrame]->GetBuffer(),sizeof(UniformCameraBufferData));

       
      



           
            //vkWaitForFences(m_Device,1,&m_DrawFences[m_CurrentFrame],true,std::numeric_limits<uint64_t>::max());

            StartRecordingCommands();

           
            VkBufferImageCopy  copyregion{};
            copyregion.bufferImageHeight = m_ColorAttachments[0]->GetHeight();
            copyregion.bufferRowLength = m_ColorAttachments[0]->GetWidth();
            copyregion.bufferOffset = 0;

            copyregion.imageExtent = { m_ColorAttachments[0]->GetWidth(),m_ColorAttachments[0]->GetHeight(),1 };
            copyregion.imageOffset = { 0,0,0 };
            copyregion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyregion.imageSubresource.mipLevel = 0;
            copyregion.imageSubresource.baseArrayLayer = 0;
            copyregion.imageSubresource.layerCount = 1;




            if (m_CurrentFrame ==1){
                vkCmdCopyImageToBuffer(m_CurrentCommandBuffer, m_ColorAttachments[0]->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, *m_PickingImageBuffer->GetBuffer(), 1, &copyregion);
            }
            
       

    }
    void Renderer::CreatePickingImage(){
        BufferDesc PickingImageBufferDesc{};
        PickingImageBufferDesc.SizeBytes = m_ColorAttachments[0]->GetByteSize();
        PickingImageBufferDesc.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        PickingImageBufferDesc.Sharingmode = VK_SHARING_MODE_EXCLUSIVE;
        PickingImageBufferDesc.Physdevice = m_PhysicalDevice;
        PickingImageBufferDesc.Device = m_Device;
        PickingImageBufferDesc.Memoryflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT|VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        m_PickingImageBuffer = new Buffer(PickingImageBufferDesc);
    }
    void Renderer::BeginGUIFrame()
    {

        FlushGeometry();
        m_StaggingBufferGeometry[m_CurrentVertexBufferQuadIndex]->UploadToBuffer(m_Device, m_VerticesQuad, sizeof(Vertex) * m_VertexPointerQuad);
          
     
       m_GUIRendering = true;
         
        
        m_VertexPointerQuad =0;
        m_VertexCount = m_VertexMaxCountGUI;

        //Swap the buffers to allow normal rendering;
        Vertex* temp = m_VerticesQuad;
        m_VerticesQuad = m_VerticesGUI;
        m_VerticesGUI = temp;
    }


   
   
    void Renderer::CreateNewBufferForBatch(std::vector<Buffer*>& VertexBuffers, std::vector<Buffer*>& Stagging)
    {
        

        VertexBuffers.push_back( new Buffer(VertexBuffers[0]->GetBufferDesc()));
        Stagging.push_back( new Buffer(Stagging[0]->GetBufferDesc()));
      
        
    }   
    void Renderer::FlushGUI()
    {
    
        auto& textures = m_Textures[m_CurrentFrame];
        auto& textureIds = m_TextureIDByOrder[m_CurrentFrame];
        m_DescriptorSetTextures[m_CurrentFrame].WriteToTexture(0,1 ,m_BlankWhiteTexture->GetImageView(), m_BlankWhiteTexture->GetSampler());


        for (uint32_t i = 0; i < textureIds.size(); i++) {
            TextureRenderingData textureData = textures[textureIds[i]];
            
          if (textureData.texture){
                Texture* texture = (Texture*)textureData.texture.GetData();

                if(texture->GetImageLayout() == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                   m_DescriptorSetTextures[m_CurrentFrame].WriteToTexture(textureData.Index,1 ,texture->GetImageView(), texture->GetSampler());
                   
                   else
                   Core::Log("Image image layout is not valid");
                if(texture->GetFormat() != VK_FORMAT_R8G8B8A8_UNORM)
                    Core::Log("FOrmat",(uint32_t)texture->GetFormat());
            }
            else{
                if(m_AssetManager->HasAsset(textureIds[i])){
                    Core::Log(ErrorType::Warning,"Manager has the asset but isn't loaded in renderer");
                }else
                    Core::Log(ErrorType::Error,"Texture rendered count ",textureIds.size());
                    Core::Log(ErrorType::Error,"Texture isn't loaded at all.{",textureIds[i].ID,"}");
            }
        }

        m_DrawCommandsGUI.push_back({ m_VertexPointerQuad-m_VertexBufferOffsetGUI,m_CurrentVertexBufferIndexGUI,m_VertexBufferOffsetGUI,m_DrawCallCountGUI + m_DrawCallCountGeometry });

        m_VertexCountPerDrawCall += m_VertexPointerQuad;
        if (m_VertexPointerQuad >= m_VertexGUIRemaining) {
           m_StaggingBufferGUI[m_CurrentVertexBufferIndexGUI]->UploadToBuffer(m_Device, m_VerticesQuad, sizeof(Vertex) * m_VertexMaxCountGUI);
            if(m_VertexCountPerDrawCall >=m_VertexBufferGUI.size()*m_VertexMaxCountGUI )
                 CreateNewBufferForBatch(m_VertexBufferGUI, m_StaggingBufferGUI);
            m_CurrentVertexBufferIndexGUI++;
            m_VertexGUIRemaining = m_VertexMaxCountGUI;
            m_VertexBufferOffsetGUI = 0;
            m_VertexPointerQuad=0;
        }
        else {
            m_VertexGUIRemaining -= (m_VertexPointerQuad-m_VertexBufferOffsetGUI);
        }
       

        m_VertexBufferOffsetGUI+=(m_VertexPointerQuad-m_VertexBufferOffsetGUI);
        m_VertexCount = m_VertexGUIRemaining;

        m_DrawCallCountGUI++;

        textureIds.clear();
    }
    
    void Renderer::FlushGeometry()
    {

        if (m_GUIRendering) {
            
            FlushGUI();
            return;
        }   
        std::vector<GUUID>& textureIds = m_TextureIDByOrder[m_CurrentFrame];
        auto& textures = m_Textures[m_CurrentFrame];
    
        //set the first descriptor to white texture for drawing without textures
        m_DescriptorSetTextures[m_CurrentFrame].WriteToTexture(0,1 ,m_BlankWhiteTexture->GetImageView(), m_BlankWhiteTexture->GetSampler());



        for (uint32_t i = 0; i < textureIds.size(); i++) {
            TextureRenderingData textureData = textures[textureIds[i]];
            
            if (textureData.texture){
                Texture* texture = (Texture*)textureData.texture.GetData();
                m_DescriptorSetTextures[m_CurrentFrame].WriteToTexture(textureData.Index,1 ,texture->GetImageView(), texture->GetSampler());
            }
            else{
                if(m_AssetManager->HasAsset(textureIds[i])){
                    Core::Log(ErrorType::Warning,"Manager has the asset but isn't loaded in renderer");
                }else
                    Core::Log(ErrorType::Error,"Texture rendered count ",textureIds.size());
                    Core::Log(ErrorType::Error,"Texture isn't loaded at all.{",textureIds[i].ID,"}");
            }
        }


        m_DrawCommandsGeometry.push_back({ m_VertexPointerQuad-m_VertexBufferOffset,m_CurrentVertexBufferQuadIndex,m_VertexBufferOffset,m_DrawCallCountGUI + m_DrawCallCountGeometry });
        m_VertexCountPerDrawCall += m_VertexPointerQuad;
        if (m_VertexPointerQuad >= m_VertexCountRemaining) {
            m_StaggingBufferGeometry[m_CurrentVertexBufferQuadIndex]->UploadToBuffer(m_Device, m_VerticesQuad, sizeof(Vertex) * m_VertexCount);
         
            if(m_VertexCountPerDrawCall >=m_VertexBufferGeometry.size()*m_VertexCount ){
                 CreateNewBufferForBatch(m_VertexBufferGeometry, m_StaggingBufferGeometry);
            }
            m_CurrentVertexBufferQuadIndex++;
            m_VertexCountRemaining = m_VertexCount;
            m_VertexBufferOffset = 0;
            m_VertexPointerQuad = 0;

            
        }
        else {
            m_VertexCountRemaining -= (m_VertexPointerQuad-m_VertexBufferOffset);

        }

            m_VertexBufferOffset += (m_VertexPointerQuad-m_VertexBufferOffset);


            m_DrawCallCountGeometry++;
           textureIds.clear();
        }
    
           
void Renderer::TransferParticleDataToBuffer(){
    m_StaggingBufferGeometry[m_CurrentVertexBufferQuadIndex]->UploadToBuffer(m_Device,m_Particles,m_CurrentParticleIndex*sizeof(InstanceParticleData),m_VertexBufferOffset*sizeof(Vertex));

       VkBufferCopy region{};
            region.size = sizeof(InstanceParticleData) * m_CurrentParticleIndex;
            region.dstOffset = m_VertexBufferOffset*sizeof(Vertex);
            region.srcOffset = m_VertexBufferOffset*sizeof(Vertex);
    vkCmdCopyBuffer(m_CurrentCommandBuffer, *m_StaggingBufferGeometry[m_CurrentVertexBufferQuadIndex]->GetBuffer(), *m_VertexBufferGeometry[m_CurrentVertexBufferQuadIndex]->GetBuffer(), 1, &region);
}
void Renderer::SubmitDrawParticleCommands(){
    VkDeviceSize offset{m_VertexBufferOffset*sizeof(Vertex)};

    vkCmdBindPipeline(m_CurrentCommandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,m_ParticlePipeline);



    vkCmdBindIndexBuffer(m_CurrentCommandBuffer,*m_IndexBuffersQuad->GetBuffer(),0,VK_INDEX_TYPE_UINT32);
    vkCmdBindVertexBuffers(m_CurrentCommandBuffer,0,1,m_VertexBufferGeometry[m_CurrentVertexBufferQuadIndex]->GetBuffer(),&offset);

    vkCmdDrawIndexed(m_CurrentCommandBuffer,m_CurrentParticleIndex*1.5f,1,0,0,0);
}

        void Renderer::EndFrame()
        {
             
              VkBufferCopy region{};
            region.size = sizeof(Vertex) * m_VertexCount;
            region.dstOffset = 0;
            region.srcOffset = 0;
            

                      for(uint32_t i =0;i < m_VertexBufferGeometry.size();i++)
                        vkCmdCopyBuffer(m_CurrentCommandBuffer, *m_StaggingBufferGeometry[i]->GetBuffer(), *m_VertexBufferGeometry[i]->GetBuffer(), 1, &region);



                        
                        
                        m_StaggingBufferGUI[m_CurrentVertexBufferIndexGUI]->UploadToBuffer(m_Device, m_VerticesQuad, sizeof(Vertex) * m_VertexMaxCountGUI);
                        region.size = sizeof(Vertex)*m_VertexMaxCountGUI;
                        
                        FlushGUI();
            for(uint32_t i=0 ;i < m_StaggingBufferGUI.size();i++)
              vkCmdCopyBuffer(m_CurrentCommandBuffer, *m_StaggingBufferGUI[i]->GetBuffer(), *m_VertexBufferGUI[i]->GetBuffer(), 1, &region);

            TransferParticleDataToBuffer();
            
            VkClearValue ClearColor[] = { {m_ClearColor.r,m_ClearColor.g,m_ClearColor.b,m_ClearColor.a},{0.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f,0.0f} };

            VkRenderPassBeginInfo RenderPassBeginInfo{};
            RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            RenderPassBeginInfo.renderPass = m_RenderPass;
            RenderPassBeginInfo.renderArea.offset = { 0,0 };
            RenderPassBeginInfo.renderArea.extent = m_SwapChain->GetExtent();
            RenderPassBeginInfo.pClearValues = ClearColor;          
            RenderPassBeginInfo.clearValueCount = 3;
            RenderPassBeginInfo.framebuffer = m_FrameBuffers[m_CurrentFrame].GetFrameBuffer(0);

            vkCmdBeginRenderPass(m_CurrentCommandBuffer, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
          
    
            DrawBatch();

            SubmitDrawParticleCommands();
            vkCmdEndRenderPass(m_CurrentCommandBuffer);
            StopRecordingCommands();




            VkSwapchainKHR swapchain = m_SwapChain->GetSwapChain();

            VkPipelineStageFlags waitstages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

            VkCommandBuffer commandsbuffers[] = { m_CommandBuffers[m_CurrentFrame] ,m_TransferCommandBuffers[m_CurrentFrame]};


            VkSubmitInfo submitinfo{};
            submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitinfo.waitSemaphoreCount = 1;
            submitinfo.pWaitSemaphores = &m_ImageAvailS[m_CurrentFrame];
            submitinfo.pWaitDstStageMask = waitstages;
            submitinfo.commandBufferCount = 2;
            submitinfo.pCommandBuffers = commandsbuffers;
            submitinfo.signalSemaphoreCount = 1;
            submitinfo.pSignalSemaphores = &m_RenderFinishedS[m_ImageIndex];
            vkResetFences(m_Device,1,&m_DrawFences[m_CurrentFrame]);
            VkResult result = vkQueueSubmit(m_GraphicsQ, 1, &submitinfo, m_DrawFences[m_CurrentFrame]);
            

            
            if (result != VK_SUCCESS)
                Core::Log(ErrorType::Error, "Failed to submit queue ",(int)result);




            
            VkPresentInfoKHR presentinfo{};
            presentinfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentinfo.waitSemaphoreCount = 1;
            presentinfo.pWaitSemaphores = &m_RenderFinishedS[m_ImageIndex];
            presentinfo.swapchainCount = 1;
            presentinfo.pSwapchains = &swapchain;
            presentinfo.pImageIndices = &m_ImageIndex;

            result = vkQueuePresentKHR(m_PresentationQ, &presentinfo);

            if (result == VK_ERROR_OUT_OF_DATE_KHR|| result == VK_SUBOPTIMAL_KHR){
                 ResizeWindow();
            }else if(result !=VK_SUCCESS ){
                 Core::Log(ErrorType::Error, "Failed to queue present ",(int)result);
            }

              m_VertexPointerQuad =0;

               m_Textures[m_CurrentFrame].clear();
            m_TextureIDByOrder[m_CurrentFrame].clear();



            m_Textures[m_CurrentFrame].clear();
            m_TextureIDByOrder[m_CurrentFrame].clear();

            m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAME_DRAWS;
            m_GUIRendering = false;

            m_CurrentCameraDescriptorSetOffset =0;
            m_CurrentTextureDescriptorSetOffset= 1;


            memset(m_VerticesQuad,0,sizeof(Vertex)*m_VertexCount);
            m_VertexBufferOffsetGUI =0;
            m_VertexBufferOffset =0;
            m_VertexPointerQuad = 0;
            m_CurrentVertexBufferIndexGUI=0;;
            m_VertexGUIRemaining=m_VertexMaxCountGUI;

            m_CurrentParticleIndex = 0;

            //delete the next upcoming rendered frame temps buffs
            if(vkGetFenceStatus(m_Device,m_DrawFences[m_CurrentFrame]) == VK_FALSE){
            auto temp = m_TempBuffers[m_CurrentFrame];
           for(uint32_t i=0;i < m_TempBuffers[m_CurrentFrame].size();i++){
               delete temp[i];
            }
            m_TempBuffers[m_CurrentFrame].clear();
             }

            
      
        }
    

    void Renderer::DrawQuad(Float3 Position, Float4 Color, Float2 Size, GUUID TextureHandle, uint64_t ID,int TextureIndex)
    {
        GUUID CurrentTextureHandle{};
        Asset<Texture> TexutreAsset{};
        uint32_t TextureID{};
        //texture map gets current container
        auto& textures = m_Textures[m_CurrentFrame];
        std::vector<GUUID>& textureIds = m_TextureIDByOrder[m_CurrentFrame];
        if (m_VertexPointerQuad + 4 > m_VertexCount )
            FlushGeometry();

        if (TextureHandle != 0) {
            TexutreAsset = m_AssetManager->GetAsset<Texture>(TextureHandle);



                switch (TexutreAsset.GetType()) {
                case AssetType::TEXTURE: {
                    CurrentTextureHandle = TextureHandle;

                    m_VerticesQuad[m_VertexPointerQuad].TexCoords = { 0.0f,1.0f };
                    m_VerticesQuad[m_VertexPointerQuad + 1].TexCoords = { 0.0f,0.0f };
                    m_VerticesQuad[m_VertexPointerQuad + 2].TexCoords = { 1.0f,0.0f };
                    m_VerticesQuad[m_VertexPointerQuad + 3].TexCoords = { 1.0f,1.0f };
                    break;
                }
                case AssetType::TEXTUREATLAS:{
                    Texture* texture = (Texture*)TexutreAsset.GetData();

                    CurrentTextureHandle = TextureHandle;
                    if(TextureIndex < texture->GetTextureCount()){

                         m_VerticesQuad[m_VertexPointerQuad].TexCoords = texture->GetTextureCoords(TextureIndex)->Coords[0];
                         m_VerticesQuad[m_VertexPointerQuad + 1].TexCoords = texture->GetTextureCoords(TextureIndex)->Coords[1];
                         m_VerticesQuad[m_VertexPointerQuad + 2].TexCoords = texture->GetTextureCoords(TextureIndex)->Coords[2];
                         m_VerticesQuad[m_VertexPointerQuad + 3].TexCoords = texture->GetTextureCoords(TextureIndex)->Coords[3];
                    }else{
                          m_VerticesQuad[m_VertexPointerQuad].TexCoords = { 0.0f,1.0f };
                          m_VerticesQuad[m_VertexPointerQuad + 1].TexCoords = { 0.0f,0.0f };
                          m_VerticesQuad[m_VertexPointerQuad + 2].TexCoords = { 1.0f,0.0f };
                          m_VerticesQuad[m_VertexPointerQuad + 3].TexCoords = { 1.0f,1.0f };
                    }
                     break;
                }
                case AssetType::ANIMATION: {
                    Core::Log(ErrorType::Error, "Not implemented yet");
                }
                default: {
                    Core::Log(ErrorType::Error, "Invalid asset type");
                }
                }

                auto index = textures.find(CurrentTextureHandle);
                if (index== textures.end()) {
                    if (textures.size() == m_TextureSlotCount - 1){
                        for(auto it = textures.begin();it != textures.end();it++){
                        }
                        FlushGeometry();
                    }
                        TexutreAsset= m_AssetManager->GetAsset<Texture>(CurrentTextureHandle);
                        textures[CurrentTextureHandle] = { TexutreAsset ,m_CurrentTextureDescriptorSetOffset};
                        textureIds.push_back(CurrentTextureHandle);
                        
                        TextureID= m_CurrentTextureDescriptorSetOffset;
                        m_CurrentTextureDescriptorSetOffset++;


                }else{

                    TextureID = index->second.Index;
                }
                uint32_t RendererTextureIndex = textures[CurrentTextureHandle].Index;
                m_VerticesQuad[m_VertexPointerQuad].TextureID = TextureID;
                m_VerticesQuad[m_VertexPointerQuad + 1].TextureID = TextureID;
                m_VerticesQuad[m_VertexPointerQuad + 2].TextureID = TextureID;
                m_VerticesQuad[m_VertexPointerQuad + 3].TextureID = TextureID;  
        }else{
                m_VerticesQuad[m_VertexPointerQuad].TextureID = TextureID;
                m_VerticesQuad[m_VertexPointerQuad + 1].TextureID = TextureID;
                m_VerticesQuad[m_VertexPointerQuad + 2].TextureID = TextureID;
                m_VerticesQuad[m_VertexPointerQuad + 3].TextureID = TextureID;
        }
              
         if(TextureHandle != 0&&m_CurrentTextureDescriptorSetOffset ==0)
            Core::Log("NOt zero ",m_CurrentTextureDescriptorSetOffset);
       

        m_VerticesQuad[m_VertexPointerQuad].Position = { Position.x - Size.x,Position.y - Size.y };
        m_VerticesQuad[m_VertexPointerQuad + 1].Position = { Position.x - Size.x,Position.y + Size.y };
        m_VerticesQuad[m_VertexPointerQuad + 2].Position = { Position.x + Size.x,Position.y + Size.y };
        m_VerticesQuad[m_VertexPointerQuad + 3].Position = { Position.x + Size.x,Position.y - Size.y };

        m_VerticesQuad[m_VertexPointerQuad].Color = Color;
        m_VerticesQuad[m_VertexPointerQuad + 1].Color = Color;
        m_VerticesQuad[m_VertexPointerQuad + 2].Color = Color;
        m_VerticesQuad[m_VertexPointerQuad + 3].Color = Color;



        m_VerticesQuad[m_VertexPointerQuad].ID = ID;
        m_VerticesQuad[m_VertexPointerQuad + 1].ID = ID;
        m_VerticesQuad[m_VertexPointerQuad + 2].ID = ID;
        m_VerticesQuad[m_VertexPointerQuad + 3].ID = ID;

           
   

        m_VertexPointerQuad += 4;


    }

  

    void Renderer::DrawQuad(Float3 Position, Float4 Color, Float2 Size, Animator Animation, uint64_t ID)
    {
        //texture map gets current container
        auto& textures = m_Textures[m_CurrentFrame];
        Asset<Texture> textureAsset = Animation.GetCurrentTexture();

            #ifdef DEBUG
                if(!textureAsset){
                   Core::Log(ErrorType::Error,"Animation texture is invalid{DrawQuad} AnimationID:",Animation.GetAnimationID().ID);
                   return;
                 }
            #endif

        GUUID textureID = textureAsset.GetID();
        std::vector<GUUID>& textureIds = m_TextureIDByOrder[m_CurrentFrame];

        if (m_VertexPointerQuad + 4 > m_VertexCount)
            FlushGeometry();
            if (textures.find(textureID) == textures.end()) {
                if (textures.size() == m_TextureSlotCount - 1)
                    FlushGeometry();

                    textures[textureID] = { Animation.GetCurrentTexture() ,m_CurrentTextureDescriptorSetOffset};
                    textureIds.push_back(textureID);
                    m_CurrentTextureDescriptorSetOffset++;

            }
            TextureRenderingData textureData = textures[textureID];
            Texture* texture = (Texture*)textureData.texture.GetData();
            if (!texture)
                return;
            m_VerticesQuad[m_VertexPointerQuad].TextureID = textureData.Index;
            m_VerticesQuad[m_VertexPointerQuad + 1].TextureID = textureData.Index;
            m_VerticesQuad[m_VertexPointerQuad + 2].TextureID = textureData.Index;
            m_VerticesQuad[m_VertexPointerQuad + 3].TextureID = textureData.Index;


            m_VerticesQuad[m_VertexPointerQuad].TexCoords = texture->GetTextureCoords(Animation.GetTextureIndex())->Coords[0];
            m_VerticesQuad[m_VertexPointerQuad + 1].TexCoords = texture->GetTextureCoords(Animation.GetTextureIndex())->Coords[1];
            m_VerticesQuad[m_VertexPointerQuad + 2].TexCoords = texture->GetTextureCoords(Animation.GetTextureIndex())->Coords[2];
            m_VerticesQuad[m_VertexPointerQuad + 3].TexCoords = texture->GetTextureCoords(Animation.GetTextureIndex())->Coords[3];


      
  
        m_VerticesQuad[m_VertexPointerQuad].Position = { Position.x - Size.x,Position.y - Size.y };
        m_VerticesQuad[m_VertexPointerQuad + 1].Position = { Position.x - Size.x,Position.y + Size.y };
        m_VerticesQuad[m_VertexPointerQuad + 2].Position = { Position.x + Size.x,Position.y + Size.y };
        m_VerticesQuad[m_VertexPointerQuad + 3].Position = { Position.x + Size.x,Position.y - Size.y };




        m_VerticesQuad[m_VertexPointerQuad].Color = Color;
        m_VerticesQuad[m_VertexPointerQuad + 1].Color = Color;
        m_VerticesQuad[m_VertexPointerQuad + 2].Color = Color;
        m_VerticesQuad[m_VertexPointerQuad + 3].Color = Color;



        m_VerticesQuad[m_VertexPointerQuad].ID = ID;
        m_VerticesQuad[m_VertexPointerQuad + 1].ID = ID;
        m_VerticesQuad[m_VertexPointerQuad + 2].ID = ID;
        m_VerticesQuad[m_VertexPointerQuad + 3].ID = ID;




        m_VertexPointerQuad += 4;

    }

  

    void Renderer::DrawQuad(Float3 Position, Float4 Color, Float2 Size, uint64_t ID)
    {
        if (m_VertexPointerQuad + 4 > m_VertexCount)
            FlushGeometry();

        m_VerticesQuad[m_VertexPointerQuad].Position = { Position.x - Size.x,Position.y - Size.y };
        m_VerticesQuad[m_VertexPointerQuad + 1].Position = { Position.x - Size.x,Position.y + Size.y };
        m_VerticesQuad[m_VertexPointerQuad + 2].Position = { Position.x + Size.x,Position.y + Size.y };
        m_VerticesQuad[m_VertexPointerQuad + 3].Position = { Position.x + Size.x,Position.y - Size.y };

        m_VerticesQuad[m_VertexPointerQuad].TextureID = 0;
        m_VerticesQuad[m_VertexPointerQuad + 1].TextureID = 0;
        m_VerticesQuad[m_VertexPointerQuad + 2].TextureID = 0;
        m_VerticesQuad[m_VertexPointerQuad + 3].TextureID = 0;


        m_VerticesQuad[m_VertexPointerQuad].Color = Color;
        m_VerticesQuad[m_VertexPointerQuad + 1].Color = Color;
        m_VerticesQuad[m_VertexPointerQuad + 2].Color = Color;
        m_VerticesQuad[m_VertexPointerQuad + 3].Color = Color;



        m_VerticesQuad[m_VertexPointerQuad].ID = ID;
        m_VerticesQuad[m_VertexPointerQuad + 1].ID = ID;
        m_VerticesQuad[m_VertexPointerQuad + 2].ID = ID;
        m_VerticesQuad[m_VertexPointerQuad + 3].ID = ID;

            m_VerticesQuad[m_VertexPointerQuad].TexCoords = { 0.0f,1.0f };
            m_VerticesQuad[m_VertexPointerQuad + 1].TexCoords = { 0.0f,0.0f };
            m_VerticesQuad[m_VertexPointerQuad + 2].TexCoords = { 1.0f,0.0f };
            m_VerticesQuad[m_VertexPointerQuad + 3].TexCoords = { 1.0f,1.0f };
     


        m_VertexPointerQuad += 4;
    }

    void Renderer::SetCurrentFont(Asset<Font> FontAsset)
    {
        
        m_CurrentFont = FontAsset;
       

   
    }
    float Renderer::GetFONTDPI(){
        return 72;
    }
    void Renderer::RenderText(const char* Message, Float2 Position, Float2 BoundingBox[4], float FixedPadding,float CharSizePixels,GUUID id,int64_t PointerIndex)
    {

        //Char being edited index
        if (m_CurrentFont.GetType() != AssetType::FONT)
        {
            Core::Log(ErrorType::Error, "Invalid type must be font{",(uint32_t)m_CurrentFont.GetType(),"}");
            return;
        }

        auto& textures = m_Textures[m_CurrentFrame];
        std::vector<GUUID>& textureIds = m_TextureIDByOrder[m_CurrentFrame];
        Font* font = (Font*)m_CurrentFont.GetData();
        Texture* FontAtlasTexture{};
        uint64_t stringLen{strlen(Message)};

        Float4 Color{ 1.0f,1.0f,1.0f,1.0f };
        float CharSizeNorm{};

        float SpaceBetweenLines{ };

        float OffsetX{ FixedPadding  };
        float OffsetY{ };
        Float2 MinCord{}, MaxCord{};
        GUUID TextureHandle = font->TextureID;
        float Space{ 0.06f };
        Float2 advance{};
        float penPosX = (Position.x * 0.5f + 0.5f) * GetViewPortExtent().width;
        float penPosY = (0.5f - Position.y * 0.5f) * GetViewPortExtent().height+(CharSizePixels*1.25f);
        if(font->TextureAsset.GetType() != AssetType::TEXTURE){
            Core::Log(ErrorType::Error, "Invalid type must be texture{",(uint32_t)font->TextureAsset.GetType(),"}");
            return;
        }
        FontAtlasTexture = (Texture*)font->TextureAsset.GetData();

         CharSizeNorm = CharSizePixels / FontAtlasTexture->GetWidth();
         SpaceBetweenLines={ (CharSizePixels / FontAtlasTexture->GetHeight()) * .25f };
         OffsetY={ SpaceBetweenLines+ CharSizeNorm };


     
        if (textures.size() == m_TextureSlotCount - 1|| m_VertexPointerQuad>= m_VertexMaxCountGUI)
            FlushGeometry();
        if (textures.find(TextureHandle) == textures.end()) {
            textures[TextureHandle] = { font->TextureAsset ,m_CurrentTextureDescriptorSetOffset };
            textureIds.push_back(TextureHandle);
            m_CurrentTextureDescriptorSetOffset++;
        }

        if(stringLen == 0 && PointerIndex != -1){
                   Float2 ndcPenPos = Core::ToNDC({penPosX,penPosY-(CharSizePixels*0.25f)});
                Float2 sizeNDC = {(float)m_CurrentFont.GetData()->FontSize/(float)GetViewPortExtent().width*0.15f,(float)m_CurrentFont.GetData()->FontSize/GetViewPortExtent().height*1.25f};
                DrawQuad({ndcPenPos.x,ndcPenPos.y,0.0f}, m_ArrowColor, sizeNDC, 0);
        }

        //Do this for every letter
        float Max{};
        float Min{};
        for (uint32_t i = 0; i <= stringLen; i++) {
            Float2 SubTextureSize{};
            Float2 Size{};
            int32_t LetterIndex = Message[i];

          
               //draw pointer
            if(PointerIndex ==i  ){
                Float2 ndcPenPos = Core::ToNDC({penPosX,penPosY-(CharSizePixels*0.25f)});
                Float2 sizeNDC = {(float)m_CurrentFont.GetData()->FontSize/(float)GetViewPortExtent().width*0.15f,(float)m_CurrentFont.GetData()->FontSize/GetViewPortExtent().height*1.25f};
                DrawQuad({ndcPenPos.x,ndcPenPos.y,0.0f}, m_ArrowColor, sizeNDC, 0);
            }
            if(LetterIndex >= font->GlyphCount)
                continue;
            //edge cases
            //Special cases
            switch (Message[i]) {
            case ' ': {
                //skip this letter
                penPosX += font->Advance[LetterIndex].x;
                continue;
            }
            case '\n': {
                penPosY += 1.25f*+m_CurrentFont.GetData()->FontSize;
                penPosX = (Position.x * 0.5f + 0.5f) * GetViewPortExtent().width;
                continue;
            }
            case '\0':{
                return;
            }
            }
            //space letter index ==-1

            if (m_VertexPointerQuad>= m_VertexMaxCountGUI)
                FlushGeometry();
          
            TextureRenderingData texture = textures[TextureHandle];
                //its the size of the bitmap not the character itself.

            m_VerticesQuad[m_VertexPointerQuad].TextureID = texture.Index;
            m_VerticesQuad[m_VertexPointerQuad + 1].TextureID = texture.Index;
            m_VerticesQuad[m_VertexPointerQuad + 2].TextureID = texture.Index;
            m_VerticesQuad[m_VertexPointerQuad + 3].TextureID = texture.Index;
            if (LetterIndex != -1) {
                m_VerticesQuad[m_VertexPointerQuad].TexCoords = font->Coords[LetterIndex].Coords[0];
                m_VerticesQuad[m_VertexPointerQuad + 1].TexCoords = font->Coords[LetterIndex].Coords[1];
                m_VerticesQuad[m_VertexPointerQuad + 2].TexCoords = font->Coords[LetterIndex].Coords[2];
                m_VerticesQuad[m_VertexPointerQuad + 3].TexCoords = font->Coords[LetterIndex].Coords[3];

            }
            else {
                m_VerticesQuad[m_VertexPointerQuad].TexCoords = {0.0f,0.0f };
                m_VerticesQuad[m_VertexPointerQuad + 1].TexCoords = { 1.0f,0.0f };
                m_VerticesQuad[m_VertexPointerQuad + 2].TexCoords = { 1.0f,1.0f };
                m_VerticesQuad[m_VertexPointerQuad + 3].TexCoords = { 0.0f,1.0f };
            }
 

            float RemainingOffset{};

            RemainingOffset = CharSizePixels - font->Coords[LetterIndex].Width;
            if (RemainingOffset > 0)
                OffsetX += (RemainingOffset * 0.5f) / FontAtlasTexture->GetWidth();

            MaxCord = font->MaxCord[LetterIndex];
            MinCord = font->MinCord[LetterIndex];
            advance = font->Advance[LetterIndex];

          

            float baselineY = penPosY; // in pixels

            baselineY -= MinCord.y;
            baselineY -= MaxCord.y;
            MinCord.y = baselineY+MinCord.y;
            MaxCord.y = baselineY+MaxCord.y;

            MinCord.x = penPosX;
            MaxCord.x = penPosX+advance.x;

            Float2 glyphPosPixelMin = Core::ToNDC(MinCord);
            Float2 glyphPosPixelMax =Core::ToNDC(MaxCord);

            //stop Rendering if it hits bottom boundrie
            if(glyphPosPixelMin.y < BoundingBox[0].y){
                return;
            }
            //put on new line if exeeceds boundries
            if(glyphPosPixelMax.x > BoundingBox[3].x){
                MaxCord = font->MaxCord[LetterIndex];
                MinCord = font->MinCord[LetterIndex];

                penPosX = (Position.x * 0.5f + 0.5f) * GetViewPortExtent().width;
                penPosY += 1.0f*+m_CurrentFont.GetData()->FontSize;

                baselineY = penPosY;

                baselineY -= MinCord.y;
                baselineY -= MaxCord.y;
                MinCord.y = baselineY+MinCord.y;
                MaxCord.y = baselineY+MaxCord.y;

                MinCord.x += penPosX;
                MaxCord.x += penPosX;

                glyphPosPixelMin = Core::ToNDC(MinCord);
                glyphPosPixelMax =Core::ToNDC(MaxCord);
                if(glyphPosPixelMin.y < BoundingBox[0].y){
                         return;
                 }
            }
           
            


          m_VerticesQuad[m_VertexPointerQuad + 0].Position = {  glyphPosPixelMin.x,  glyphPosPixelMin.y, 0.0f }; // bottom-left
          m_VerticesQuad[m_VertexPointerQuad + 1].Position = {  glyphPosPixelMin.x,  glyphPosPixelMax.y, 0.0f }; // top-left
          m_VerticesQuad[m_VertexPointerQuad + 2].Position = {  glyphPosPixelMax.x,  glyphPosPixelMax.y, 0.0f }; // top-right
          m_VerticesQuad[m_VertexPointerQuad + 3].Position = {  glyphPosPixelMax.x,  glyphPosPixelMin.y, 0.0f }; // bottom-right

          penPosX += advance.x;
          //penPosY -= advance.y;

            if (RemainingOffset > 0)
                OffsetX += (RemainingOffset * 0.5f) / FontAtlasTexture->GetWidth();
    


     

          

            m_VerticesQuad[m_VertexPointerQuad].Color = Color;
            m_VerticesQuad[m_VertexPointerQuad + 1].Color = Color;
            m_VerticesQuad[m_VertexPointerQuad + 2].Color = Color;
            m_VerticesQuad[m_VertexPointerQuad + 3].Color = Color;



            m_VerticesQuad[m_VertexPointerQuad].ID = id.ID;
            m_VerticesQuad[m_VertexPointerQuad + 1].ID = id.ID;
            m_VerticesQuad[m_VertexPointerQuad + 2].ID = id.ID;
            m_VerticesQuad[m_VertexPointerQuad + 3].ID = id.ID;





            m_VertexPointerQuad += 4;
          
           
        }
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
    void Renderer::ReCreateFrameBuffers(){
        VkFormat format = Core::ChooseBestFormat(m_PhysicalDevice, { VK_FORMAT_R32G32_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
        VkFormat DepthStencilFormat = Core::ChooseBestFormat(m_PhysicalDevice, { VK_FORMAT_D32_SFLOAT_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        TextureCreateInfo colorAttachTextureInfo{};
        colorAttachTextureInfo.Format =format;
        colorAttachTextureInfo.Width = m_SwapChain->GetExtent().width;
        colorAttachTextureInfo.Height = m_SwapChain->GetExtent().height;
        colorAttachTextureInfo.ImageTilling =  VK_IMAGE_TILING_OPTIMAL;
        colorAttachTextureInfo.ImageUsageFlags = VkImageUsageFlagBits(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
        colorAttachTextureInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        colorAttachTextureInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;


            VkCommandBuffer singleCommandBuffer = CommandBuffer::StartSingleUseCommandBuffer(m_Context,m_GraphicsPool.GetCommandPool());
            for(uint32_t i =0;i < m_ColorAttachments.size();i++){
                delete m_ColorAttachments[i];

                m_ColorAttachments[i] = new Texture(m_Context,colorAttachTextureInfo,TextureType::ColorAttachment);
                m_ColorAttachments[i]->TrasitionFormat(true,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,singleCommandBuffer);
            }
            CommandBuffer::EndSingleUseCommandBuffer(m_Context,m_GraphicsPool.GetCommandPool(),singleCommandBuffer);



             //change creat info for depth buffer
                colorAttachTextureInfo.ImageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                colorAttachTextureInfo.ImageUsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                colorAttachTextureInfo.Format = DepthStencilFormat;
                for(uint32_t i =0;i < m_DepthStencilAttachments.size();i++){
                delete m_DepthStencilAttachments[i];
                m_DepthStencilAttachments[i] = new Texture(m_Context,colorAttachTextureInfo,TextureType::DepthStencilAttachment);
                }


            CreateFrameBuffers();
    }
    void Renderer::CreateFrameBuffers(){

        m_FrameBuffers.resize(MAX_FRAME_DRAWS);
        for(int i =0;i < MAX_FRAME_DRAWS;i++){
        Texture* images[3]= {m_SwapChain->GetSwapChainImage(i),m_ColorAttachments[i],m_DepthStencilAttachments[i]};
                m_FrameBuffers[i].Init(m_Device,m_SwapChain,m_RenderPass,images,3);
        }


    }
    void Renderer::CreateColorAttachments(uint32_t Count,std::vector<Texture*>& colorAttachments,const TextureCreateInfo& createInfo){
        
        colorAttachments.resize(Count);
        for(uint32_t i =0;i  < colorAttachments.size();i++){
            colorAttachments[i] = new Texture(m_Context,createInfo,TextureType::ColorAttachment);
        }

    }


    void Renderer::CreateCommandBuffers(){

        m_GraphicsPool.Init(m_Device,m_QueueFamilies.Graphics);


        m_CommandBuffers.resize(MAX_FRAME_DRAWS);
        m_TransferCommandBuffers.resize(MAX_FRAME_DRAWS);
        

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
        allocinfo2.commandBufferCount =MAX_FRAME_DRAWS;

         result = vkAllocateCommandBuffers(m_Device,&allocinfo2,m_TransferCommandBuffers.data());
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
            Core::Log(ErrorType::Error,"Failed to create surface.{Renderer::CreateSurface} ",(int)result);
    }
 
    void Renderer::Statistics(bool renderGui,void* guiRenderer){
        GUIRenderer* gui = (GUIRenderer*)guiRenderer;
        if(renderGui){

            gui->Panel("GuiStatistics",{-0.7f,0.7f},{1.0f,1.0f,0.5f,1.0f},{0.3f,0.3f});

            GUI::BorderStyle style{sizeof(GUI::BorderStyle)};
            style.BorderWidth = 0.01f;
            style.DrawBorder = true;
            style.BorderColor = {1.0f,0.0f,0.0f,1.0f};
            style.BackGroundColor = {0.0f,1.0f,1.0f,1.0f};
            gui->PushStyle(GUI::Style::BORDER,&style);
            gui->Text("DrawCallCount","DRAWCALL: "+std::to_string(m_DrawCallCountGeometry+m_DrawCallCountGUI),{0.0f,0.75f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.25f});

            gui->Text("TriangleCount","TRIANGLE: "+std::to_string(m_VertexCountPerFrame/3),{0.0f,0.25f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.25f});
            gui->Text("VertexCount","VERTEX: "+std::to_string(m_VertexCountPerFrame),{0.0f,-0.25f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.25f});

            std::string deltatimeString = std::to_string(m_DeltaTime);

            uint64_t Index = deltatimeString.find_last_of(".");
            uint32_t Prec = 3;
            deltatimeString = deltatimeString.substr(0,Index+Prec);
            gui->Text("Frametime","FRAMETIME: "+deltatimeString,{0.0f,-0.75f},{1.0f,1.0f,1.0f,1.0f},{1.0f,0.25f});

            gui->PopStyle();
            gui->EndPanel();



        }else{

            Core::Log(ErrorType::Info,"Draw call count",m_DrawCallCountGeometry+m_DrawCallCountGUI+m_DrawCallCountOutlines);
        }
    }

    Texture* Renderer::LoadTexture(std::string Path,TextureType type)
    {
        TextureCreateInfo createInfo{};
        createInfo.Format = VK_FORMAT_R8G8B8A8_UNORM;
        createInfo.ImageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
        createInfo.ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        Texture*texture = new Texture(m_Context,createInfo, Path,type);
        return texture;
    }
    void Renderer::SetRenderDesc(const RendererDesc& desc){
        m_RendererDescNext = desc;
    }
    void Renderer::InitRenderDesc(const RendererDesc& Desc){
        vkDeviceWaitIdle(m_Device);
        m_RendererDesc = Desc;
        Core::Log("ReCreated");
        m_PipelineDesc.RenderPass = m_RenderPass;
        m_PipelineDesc.Viewport.width = (float)m_SwapChain->GetExtent().width;
        m_PipelineDesc.Viewport.height = (float)m_SwapChain->GetExtent().height;
        switch(m_RendererDesc.Rendermode){
            case RenderMode::SOLID:{
                m_PipelineDesc.RenderType = VK_POLYGON_MODE_FILL;
                m_PipelineDesc.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                m_PipelineDesc.Blending = true;

                
                uint32_t* Indices = new uint32_t[m_VertexCount*1.5];
                uint32_t Offset{};
                for(uint32_t i =0;i < m_VertexCount*1.5;i+=6)
                {
                    Indices[i] = Offset;
                    Indices[i+1]=Offset+1;
                    Indices[i+2] = Offset+2;
                    Indices[i+3]=Offset+2;
                    Indices[i+4] = Offset+3;
                    Indices[i+5]=Offset;
                    Offset+=4;
                }
                BufferDesc desc{};
                desc = m_IndexBuffersQuad->GetBufferDesc();
                delete m_IndexBuffersQuad;
                desc.SizeBytes = sizeof(uint32_t)*m_VertexCount*1.5f;
                m_IndexBuffersQuad = new Buffer(desc);
                m_IndexBuffersQuad->UploadToBuffer(m_Device,Indices,m_IndexBuffersQuad->GetBufferDesc().SizeBytes);
                delete[] Indices;
                break;
            }
            case RenderMode::WIREFRAME:{
                m_PipelineDesc.RenderType = VK_POLYGON_MODE_LINE;
                m_PipelineDesc.Topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                m_PipelineDesc.Blending = false;

                uint32_t* Indices = new uint32_t[m_VertexCount*2];
                uint32_t Offset{};
                for(uint32_t i =0;i < m_VertexCount*2;i+=8)
                {
                    Indices[i] = Offset;
                    Indices[i+1]=Offset+1;
                    Indices[i+2] = Offset+1;
                    Indices[i+3]=Offset+2;
                    Indices[i+4] = Offset+2;
                    Indices[i+5]=Offset+3;
                    Indices[i+6] = Offset+3;
                    Indices[i+7]=Offset;
                    Offset+=4;
                }
                BufferDesc desc{};
                desc = m_IndexBuffersQuad->GetBufferDesc();
                delete m_IndexBuffersQuad;
                desc.SizeBytes = sizeof(uint32_t)*m_VertexCount*2;
                m_IndexBuffersQuad = new Buffer(desc);
                m_IndexBuffersQuad->UploadToBuffer(m_Device,Indices,m_IndexBuffersQuad->GetBufferDesc().SizeBytes);
                delete[] Indices;
                break;
            }
            default: {
                Core::Log(ErrorType::Error,"Invalid rendermode falling back to solid");
                m_RendererDesc.Rendermode = RenderMode::SOLID;
                m_PipelineDesc.RenderType = VK_POLYGON_MODE_FILL;
                m_PipelineDesc.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                m_PipelineDesc.Blending = true;
                break;
            }
        }


        m_PipelineDesc.ShaderCount = m_CurrentlyLoadedShaders.size();
        m_PipelineDesc.ShaderModules= new VkShaderModule[m_CurrentlyLoadedShaders.size()];
        m_PipelineDesc.ShaderStages= new VkShaderStageFlagBits[m_CurrentlyLoadedShaders.size()];
        for(uint32_t i=0 ;i < m_CurrentlyLoadedShaders.size();i++){

            m_PipelineDesc.ShaderModules[i] = m_CurrentlyLoadedShaders[i].GetData()->GetShaderModule();
            m_PipelineDesc.ShaderStages[i] = m_CurrentlyLoadedShaders[i].GetData()->GetShaderStage();
        }

        ReCreatePipeline(m_PipelineDesc);
        delete[] m_PipelineDesc.ShaderStages;
        delete[] m_PipelineDesc.ShaderModules;
    }
   void Renderer::ReCreatePipeline(const PipelineDesc& desc){

        vkDestroyPipeline(m_Device,m_Pipeline,nullptr);
        m_Pipeline = Pipeline::CreatePipeline(desc,m_Device);
   }



Renderer::~Renderer(){
    Shutdown();
}

void Renderer::StartRecordingCommands()
{
 
    VkCommandBufferBeginInfo bufferbegininfo{};
    bufferbegininfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;


    vkResetCommandBuffer(m_CurrentCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    vkBeginCommandBuffer(m_CurrentCommandBuffer, &bufferbegininfo);
    vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

    vkResetCommandBuffer(m_Context->TransferCommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    vkBeginCommandBuffer(m_Context->TransferCommandBuffer,&bufferbegininfo);
}

void Renderer::StopRecordingCommands()
{
    vkEndCommandBuffer(m_Context->TransferCommandBuffer);
    
    vkEndCommandBuffer(m_CurrentCommandBuffer);
    m_DrawCommandsGeometry.resize(0);
    m_DrawCommandsOutlines.resize(0);
    m_DrawCommandsGUI.resize(0);
}

void Renderer::DrawBatch()
{
    //Debugging 
        m_VertexCountPerFrame= 0;

 
    VkDeviceSize Offset{ 0 };

    for (int i = 0; i < m_DrawCommandsGeometry.size(); i++) {
        DrawCommand DrawCall = m_DrawCommandsGeometry[i];
        VkDeviceSize VertexBufferOffset{ DrawCall.VertexBufferOffset*sizeof(Vertex)};


        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, m_VertexBufferGeometry[DrawCall.VertexBufferIndex]->GetBuffer(), &VertexBufferOffset);

        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, *m_IndexBuffersQuad->GetBuffer(), Offset, VK_INDEX_TYPE_UINT32);

        VkDescriptorSet DescriptorSets[2];
        DescriptorSets[0] = m_DescriptorSetCamera[m_CurrentFrame].GetDescriptorSet();
        DescriptorSets[1] = m_DescriptorSetTextures[m_CurrentFrame].GetDescriptorSet();
 

       
        static uint32_t uniformBufferIndex{0};
        vkCmdPushConstants(m_CurrentCommandBuffer,m_PipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,0,sizeof(uint32_t),&uniformBufferIndex);
        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 2, DescriptorSets, 0, nullptr);

        
        vkCmdDrawIndexed(m_CurrentCommandBuffer, uint32_t(DrawCall.VertexCount * 1.5f), 1, 0,0, 0);
        m_VertexCountPerFrame+= DrawCall.VertexCount;

    }

   
    for (uint32_t i = 0; i < m_DrawCommandsGUI.size(); i++) {
        DrawCommand DrawCall = m_DrawCommandsGUI[i];
        uint64_t VertexBufferOffset = {m_DrawCommandsGUI[i].VertexBufferOffset*sizeof(Vertex)};


        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, 0, 1, m_VertexBufferGUI[m_DrawCommandsGUI[i].VertexBufferIndex]->GetBuffer(), &VertexBufferOffset);

        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, *m_IndexBuffersQuad->GetBuffer(), Offset, VK_INDEX_TYPE_UINT32);

        VkDescriptorSet DescriptorSets[2];
        DescriptorSets[0] = m_DescriptorSetCamera[m_CurrentFrame].GetDescriptorSet();
        DescriptorSets[1] = m_DescriptorSetTextures[m_CurrentFrame].GetDescriptorSet();




       static  uint32_t uniformBufferIndex{1};
        vkCmdPushConstants(m_CurrentCommandBuffer,m_PipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,0,sizeof(uint32_t),&uniformBufferIndex);
        vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 2, DescriptorSets, 0, nullptr);

        vkCmdDrawIndexed(m_CurrentCommandBuffer, uint32_t(m_DrawCommandsGUI[i].VertexCount * 1.5f), 1, 0,0, 0);
        m_VertexCountPerFrame+= DrawCall.VertexCount;

    }


    m_VertexCountPerDrawCall = 0;
}

void Renderer::CreateDescriptorSets(){
    uint32_t descriptorTextureCountPerSet = 1000;
   for(uint32_t i=0;i < MAX_FRAME_DRAWS;i++){
       m_DescriptorPool.AddDescriptorType(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
       m_DescriptorPool.AddDescriptorType(descriptorTextureCountPerSet*m_DescriptorSetTextures.size(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
   }


    m_DescriptorPool.CreatePool(m_Context);

    DescriptorSetDescription CameraDescriptordesc{ m_Context };
    CameraDescriptordesc.DescriptorCount = 1;
    CameraDescriptordesc.DescriptorPool = m_DescriptorPool.GetPool();
    CameraDescriptordesc.StageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    CameraDescriptordesc.Type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    for(uint32_t i =0;i < m_DescriptorSetCamera.size();i++)
        m_DescriptorSetCamera[i].Init(CameraDescriptordesc);


    DescriptorSetDescription TextureDescriptordesc{ m_Context };
    TextureDescriptordesc.DescriptorCount = descriptorTextureCountPerSet;
    TextureDescriptordesc.DescriptorPool = m_DescriptorPool.GetPool();
    TextureDescriptordesc.StageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    TextureDescriptordesc.Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;


    
    
    
    for(uint32_t i =0;i < m_DescriptorSetTextures.size();i++){
    m_DescriptorSetTextures[i].Init(TextureDescriptordesc);
    //overwriting all descriptor to blank white .
     m_DescriptorSetTextures[i].WriteToTexture(0,m_DescriptorSetTextures[i].GetDescriptorCount(), m_BlankWhiteTexture->GetImageView(), m_BlankWhiteTexture->GetSampler());
    }

    }
void Renderer::QueueShaderChange(const std::string& path){
    if(m_QueuedShaders.capacity() == m_QueuedShaders.size())
    {
        Core::Log(ErrorType::Warning,"Reached maximum queued shader.{QueueShader::Change}",m_QueuedShaders.capacity(),m_QueuedShaders.size());
        return;
    }
    Core::Log(ErrorType::Error,"Currently not implemented");
}
void Renderer::QueueShaderChange(Asset<Shader> shaderAsset){
    if(m_QueuedShaders.capacity() == m_QueuedShaders.size())
    {
        Core::Log(ErrorType::Warning,"Reached maximum queued shader.{QueueShader::Change}",m_QueuedShaders.capacity(),m_QueuedShaders.size());
        return;
    }
    m_QueuedShaderPaths.push_back("None provided");
    m_QueuedShaders.push_back(shaderAsset);
}
void Renderer::CreateParticlePipeline(){
    VkShaderModule shaders[2];
    VkShaderStageFlagBits shaderStages[2];

    auto frag =m_AssetManager->GetAsset<Shader>("Shaders/ParticleF");
    auto vert =m_AssetManager->GetAsset<Shader>("Shaders/ParticleV");

    shaders[0] = vert.GetData()->GetShaderModule();
    shaderStages[0] = vert.GetData()->GetShaderStage();
    
    shaders[1] = frag.GetData()->GetShaderModule();
    shaderStages[1] = frag.GetData()->GetShaderStage();

    m_ParticlePipelineDesc.RenderPass = m_RenderPass;
    m_ParticlePipelineDesc.PipelineLayout = Pipeline::CreatePipelineLayout(m_Device,nullptr,0);
    
    m_ParticlePipelineDesc.ShaderCount = 2;
    m_ParticlePipelineDesc.ShaderModules =shaders;
    m_ParticlePipelineDesc.ShaderStages =shaderStages;

    m_ParticlePipelineDesc.VertexInputStride = sizeof(InstanceParticleData);
    m_ParticlePipelineDesc.VertexStageInputCount = 4;
    m_ParticlePipelineDesc.VertexStageInput = new VertexStageInputAttrib[ m_ParticlePipelineDesc.VertexStageInputCount];
    m_ParticlePipelineDesc.VertexStageInput[0] = {VK_FORMAT_R32G32B32_SFLOAT,offsetof(InstanceParticleData,ParticlePos),0,0};
    m_ParticlePipelineDesc.VertexStageInput[1] = {VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(InstanceParticleData,Color),1,0};
    m_ParticlePipelineDesc.VertexStageInput[2] = {VK_FORMAT_R32G32_SFLOAT,offsetof(InstanceParticleData,TextureCords),2,0};
    m_ParticlePipelineDesc.VertexStageInput[3] = {VK_FORMAT_R32_UINT,offsetof(InstanceParticleData,TextureID),3,0};

    //For now you same viewport.
    m_ParticlePipelineDesc.Viewport = m_PipelineDesc.Viewport;




     m_ParticlePipeline= Pipeline::CreatePipeline(m_ParticlePipelineDesc,m_Device);

     delete[] m_ParticlePipelineDesc.VertexStageInput;
}
void Renderer::RunRendererChangeQueue(){
    if(m_QueuedShaders.size() == 0)
        return;
    vkDeviceWaitIdle(m_Device);

    m_CurrentlyLoadedShaders.clear();

    m_PipelineDesc.ShaderCount =m_QueuedShaders.size();
    m_PipelineDesc.ShaderModules = new VkShaderModule[m_QueuedShaders.size()];
    m_PipelineDesc.ShaderStages = new VkShaderStageFlagBits[m_QueuedShaders.size()];
    for(uint32_t i=0;i < m_QueuedShaders.size();i++){
        m_PipelineDesc.ShaderModules[i] =m_QueuedShaders[i].GetData()->GetShaderModule();
        m_PipelineDesc.ShaderStages[i] = m_QueuedShaders[i].GetData()->GetShaderStage();
        m_CurrentlyLoadedShaders.push_back(m_QueuedShaders[i]);
    }

    ReCreatePipeline(m_PipelineDesc);



    m_QueuedShaders.clear();
    m_QueuedShaderPaths.clear();
    m_QueuedShaders.reserve(20);
   delete[] m_PipelineDesc.ShaderModules;
   delete[] m_PipelineDesc.ShaderStages;
}

void Renderer::Shutdown(){
    if(m_ShutDown)
        return; 
    m_ShutDown = true;
    vkDeviceWaitIdle(m_Device);


    //deleting all buffers.
    for(uint32_t i =0;i < m_VertexBufferGeometry.size();i++)
        delete m_VertexBufferGeometry[i];
    
        delete m_IndexBuffersQuad;
        delete m_IndexBufferVertices;
    
    for(uint32_t i=0;i < m_StaggingBufferGeometry.size();i++)
        delete m_StaggingBufferGeometry[i];

    for(uint32_t i=0;i < m_VertexBufferGUI.size();i++)
        delete m_VertexBufferGUI[i];
    for(uint32_t i=0;i < m_StaggingBufferGUI.size();i++)
        delete m_StaggingBufferGUI[i];

    for(uint32_t i=0;i < m_VertexBufferOutlines.size();i++)
        delete m_VertexBufferOutlines[i];
    for(uint32_t i=0;i < m_StaggingBufferOutlines.size();i++)
        delete m_StaggingBufferOutlines[i];
    delete m_BlankWhiteTexture;

    for(uint32_t i =0;i < m_UniformBuffer.size();i++)
        delete m_UniformBuffer[i];

    delete m_PickingImageBuffer;
    
    m_FrameBuffers.clear();
    
    for(uint32_t i =0;i < MAX_FRAME_DRAWS;i++)
    m_Textures[i].clear();
    
    for(uint32_t i=0;i < m_DepthStencilAttachments.size();i++)
    delete m_DepthStencilAttachments[i];
    for(uint32_t i=0;i < m_ColorAttachments.size();i++)
    delete m_ColorAttachments[i];
    for(uint32_t i=0;i < m_RenderFinishedS.size();i++)
        vkDestroySemaphore(m_Device,m_RenderFinishedS[i],nullptr);
    for(uint32_t i=0;i < m_ImageAvailS.size();i++)
        vkDestroySemaphore(m_Device,m_ImageAvailS[i],nullptr);
    for(uint32_t i=0;i < m_DrawFences.size();i++)
        vkDestroyFence(m_Device,m_DrawFences[i],nullptr);

    
    for(uint32_t i=0; i < m_DescriptorSetCamera.size();i++)
    vkDestroyDescriptorSetLayout(m_Device,m_DescriptorSetCamera[i].GetDescriptorLayout(),nullptr);

    for(uint32_t i=0;i < m_DescriptorSetTextures.size();i++)
    vkDestroyDescriptorSetLayout(m_Device,m_DescriptorSetTextures[i].GetDescriptorLayout(),nullptr);

    m_DescriptorPool.Destroy();
    
    vkDestroyRenderPass(m_Device,m_RenderPass,nullptr);

    vkDestroyPipelineLayout(m_Device,m_ParticlePipelineDesc.PipelineLayout,nullptr);
    vkDestroyPipelineLayout(m_Device,m_PipelineLayout,nullptr);
    vkDestroyPipeline(m_Device,m_Pipeline,nullptr);
    vkDestroyPipeline(m_Device,m_ParticlePipeline,nullptr);


    vkFreeCommandBuffers(m_Device,m_GraphicsPool.GetCommandPool(),m_CommandBuffers.size(),m_CommandBuffers.data());
    vkDestroyCommandPool(m_Device,m_GraphicsPool.GetCommandPool(),nullptr);
    
    delete m_SwapChain;
    delete m_Context;
    vkDestroySurfaceKHR(m_Instance,m_Surface,nullptr);
    vkDestroyDevice(m_Device,nullptr);

    auto destroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_Instance,"vkDestroyDebugUtilsMessengerEXT");
    destroyDebugMessenger(m_Instance,m_Messenger,nullptr);
    
    vkDestroyInstance(m_Instance,nullptr);

    delete[] m_PipelineDesc.VertexStageInput;


    delete[] m_VerticesQuad;
    delete[] m_Vertices;
    delete[] m_VerticesGUI;  
    delete[] m_Particles;

}
void Renderer::FinishExecution(){
    vkDeviceWaitIdle(m_Device);
    //release assets
    m_Textures->clear();
    m_CurrentFont.~Asset();
    m_CurrentFont= Asset<Font>();
    m_CurrentlyLoadedShaders.clear();
    
}

