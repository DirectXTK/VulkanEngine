#pragma once
#include "SwapChain.h"
#include "Shader.h"
#include "CommandPool.h"
#include "FrameBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Descriptor.h"
#include "Camera.h"
#include "Image.h"
#include "Buffer.h"
#define MAX_FRAME_DRAWS 2
struct Vertex{
    Float3 Position{};
	Float4 Color{};
	uint64_t ID{0};
	//Float2 TexCoord{ 0.0f,0.0f };
	//int TexID{ -1 };
};
struct RendererDesc{
    uint32_t VertexCountPerDrawCall{1000};
};
class Renderer{
 public:
    Renderer(RendererDesc desc,GLFWwindow* window);

    void BeginFrame(glm::mat4 viewproj);



    void DrawQuad(Float3 Position,Float4 Color,Float2 Size,Image* image,uint64_t ID);
    void DrawParticle();
    Buffer* GetCustomBuffer(uint32_t index) { return m_PickingImageBuffer; }


    void EndFrame();

    void Statistics();

    ~Renderer();
private:

    void Flush(bool LastFrame);

    void CreateInstance();
    void CreateFrameBuffers();
    void CreateCommandBuffers();

    uint32_t m_CurrentFrame{};

    VkInstance m_Instance{};
    VkPhysicalDevice m_PhysicalDevice{};
    VkDevice m_Device{};
    VkSurfaceKHR m_Surface{};
    GLFWwindow* m_Window{};
    //Swap chain stuff
    SwapChainDetails m_SwapChainDetails{};

    SwapChain* m_SwapChain{};

    uint32_t m_ImageIndex{};
    std::vector<VkCommandBuffer> m_CommandBuffers{};
    VkCommandBuffer m_TransferCommandBuffer{};
    std::vector<FrameBuffer> m_FrameBuffers{};
    std::vector<Image> m_ColorAttachments{};
    //Queues
    VkQueue m_GraphicsQ{};
    VkQueue m_PresentationQ{};
    //pools
    CommandPool m_GraphicsPool{};

    VkPipelineLayout m_PipelineLayout{};
    VkPipeline m_Pipeline{};
    VkRenderPass m_RenderPass{};

    //Syncs
    std::vector<VkSemaphore> m_ImageAvailS{};
    std::vector<VkSemaphore> m_RenderFinishedS{};
    std::vector<VkFence> m_DrawFences{};
    //Buffers
    VertexBuffer* m_VertexBuffer{};
    IndexBuffer m_IndexBuffer{};
    UniformBuffer* m_UniformBuffers{};
    //Descriptors 

    DescriptorLayout m_DescriptorLayout{};
    //Debug
    VkDebugUtilsMessengerEXT m_Messenger{};
    void CreateDebugger();


    void RecordCommands(uint32_t VertexCount,uint32_t IndexCount);

    void CreatePipeline();
    void GetPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSurface(GLFWwindow* window,VkSurfaceKHR* surface);

    QueueFamilies GetQueueFamilies(VkPhysicalDevice device);

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    bool IsDeviceSiutable(VkPhysicalDevice device);
    bool IsExtensionsSupported(std::vector<const char*> extensions);
    void CreateSamaphore();

    //One frame data
    Vertex* m_Vertices{};
    uint32_t m_VertexCount{};
    uint32_t m_VertexPointer{};
    glm::mat4 m_CameraViewProj{};
    Image* m_FrameImageIndexed{};
    //Diagnostics
    uint32_t m_DrawCallCount{};
    //TEMP(Testing)
    Buffer* m_PickingImageBuffer{};

    
};
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageseverity,VkDebugUtilsMessageTypeFlagsEXT messagetype,const VkDebugUtilsMessengerCallbackDataEXT* pcallbackdata,void* puserData );
 