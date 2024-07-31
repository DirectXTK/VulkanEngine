#pragma once
#define DEMO_FEATURES
#ifdef DEMO_FEATURES
#include "GUI.h"
#endif

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
#include "VulkanInstance.h"
#include "Texture.h"
#include "Animator.h"
#define MAX_FRAME_DRAWS 2

class InputSystem;
class AssetManager;
struct Vertex{
    Float3 Position{};
	Float4 Color{};
	uint64_t ID{0};
	Float2 TexCoords{ 0.0f,0.0f };
	uint32_t TextureID{ 0 };
};
enum class FlushType {GEOMETRY,OUTLINE,GUI};
struct DrawCommand {
    uint64_t VertexCount{};
    uint32_t DescriptorSetTextureIndex{};
};
enum class RenderFeature {NONE,DRAWOUTLINE};
struct RenderMode {
    RenderFeature Feature{ RenderFeature::NONE};
    bool Enable{ true };
};
struct RendererDesc{
    uint32_t VertexCountPerDrawCall{100};
    Float4 ClearColor{};
    Camera2D* InitialCamera{nullptr}; //optional
};
class Renderer {
public:
    //Initialization functions
    Renderer(RendererDesc desc, GLFWwindow* window, InputSystem* inputsystem, AssetManager* assetManager);
    void InitializePipeline(uint64_t MaxTextureCount);
    //

    void BeginFrame(Camera2D* camera);
    void BeginGUIFrame();

    //GUI
    //void DrawQuadGUI(Float3 Position, Float4 Color, Float2 Size, uint64_t ID);
    //void DrawQuadWithAtlasGUI(Float3 Position, Float4 Color, Float2 Size, GUUID textureatlas, uint64_t ID, uint64_t TextureIndex);
   // void DrawQuadGUI(Float3 Position, Float4 Color, Float2 Size, GUUID TextureHandle, uint64_t ID, uint64_t TextureIndex);
    //
    void DrawQuad(Float3 Position, Float4 Color, Float2 Size, GUUID TextureHandle, uint64_t ID, uint32_t TextureIndex = 0);
    //void DrawQuadWithAtlas(Float3 Position, Float4 Color, Float2 Size, GUUID textureatlas, uint64_t ID, uint64_t TextureIndex);
    void DrawQuad(Float3 Position, Float4 Color, Float2 Size, Animator Animation, uint64_t ID);

    void DrawQuad(Float3 Position, Float4 Color, Float2 Size, uint64_t ID);
    //GUI 


    void DrawOutline(Float3 Position, Float2 Size,Float4 Color, float OutlineWidth);

    //Particles
    void DrawParticle();


    Buffer* GetCustomBuffer(uint32_t index) { return m_PickingImageBuffer; }
    Buffer* GetViewportWithID();

    VkExtent2D GetViewPortExtent() { return m_SwapChain->GetExtent(); }

    void EndFrame();


    void Statistics();

    Texture* LoadTexture(std::string Path);

    Context GetContext() { return m_Context; }

    ~Renderer();
private:
    void StartRecordingCommands();
    void StopRecordingCommands();

    void DrawBatch();
    void DrawGUIBatch();


    void CreateNewBufferForBatch(std::vector<Buffer*>& VertexBuffers, std::vector<Buffer*>& Stagging);

    void FlushGeometry();
    void FlushOutlines();
    void FlushGUI();

    void CreateInstance();
    void CreateFrameBuffers();
    void CreateCommandBuffers();

    Context m_Context{};

    QueueFamilies m_QueueFamilies{};

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
    VkCommandBuffer m_CurrentCommandBuffer{};
    VkCommandBuffer m_TransferCommandBuffer{};
    std::vector<FrameBuffer> m_FrameBuffers{};
    std::vector<Image> m_ColorAttachments{};
    //
    std::vector<Image> m_DepthStencilAttachments{};

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

    std::vector<Buffer*> m_IndexBuffers{};
    uint32_t* m_Indices{};




    UniformBuffer* m_UniformBuffers{};
    //Descriptors 
    DescriptorPool m_DescriptorPool{};
    DescriptorSet m_DescriptorSetCamera{};

    //Debug
    VkDebugUtilsMessengerEXT m_Messenger{};
    void CreateDebugger();


    void RecordCommands(uint32_t VertexCount, uint32_t IndexCount);

    void CreatePipeline();
    void GetPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSurface(GLFWwindow* window, VkSurfaceKHR* surface);


    bool IsExtensionsSupported(std::vector<const char*> extensions);
    void CreateSamaphore();

    //One frame data
    Vertex* m_Vertices{};
    uint32_t m_VertexCount{};
    uint32_t m_VertexPointer{};
    glm::mat4 m_CameraViewProj{};
    Image* m_FrameImageIndexed{};
    //Diagnostics
    uint32_t m_DrawCallCountGeometry{};
    uint32_t m_DrawCallCountOutlines{};
    uint32_t m_DrawCallCountGUI{};

    //TEMP(Testing)
    Buffer* m_PickingImageBuffer{};

    //FrameBuffer stuffs
    Float4 m_ClearColor{};
    //Batch renderer

    std::vector<DrawCommand> m_DrawCommandsGeometry{};
    std::vector<DrawCommand> m_DrawCommandsGUI{};
    std::vector<DrawCommand> m_DrawCommandsOutlines{};

    std::vector<Buffer*> m_VertexBufferGeometry{};
    std::vector<Buffer*> m_VertexBufferGUI{};
    std::vector<Buffer*> m_VertexBufferOutlines{};

    std::vector<Buffer*> m_StaggingBufferGeometry{};
    std::vector<Buffer*> m_StaggingBufferGUI{};
    std::vector<Buffer*> m_StaggingBufferOutlines{};

    //Outlines
    uint32_t m_VertexCountOutlines{};
    uint64_t m_VertexOutineMaxCountPerDrawCall{ 4 * 100 };
    Vertex* m_VertexOutline{};






    Camera2D m_Camera{};
    //Texturing
    uint32_t m_TextureSlotCount{ 4 };
    Texture* m_BlankWhiteTexture{};
    DescriptorPool m_DescriptorPoolTextures{};
    std::vector<DescriptorSet> m_DescriptorSetTextures{};
    struct TextureRenderingData {
        Texture* texture{};
        uint32_t Index{};
    };
    std::unordered_map<GUUID, TextureRenderingData> m_Textures{};
    std::vector<GUUID> m_TextureIDByOrder{};
    //GUI stuff
    Vertex* m_VerticesGUI{};
    uint64_t m_VertexMaxCountGUI{ 100 * 4 };
    uint64_t m_VertexCountGUI{};
    DescriptorSet m_GUICameraDescriptor{};
    Buffer* m_UniformGUICameraBuffer{};
    std::vector<DescriptorSet> m_DescriptorSetTexturesGUI{};
    std::unordered_map<GUUID, TextureRenderingData> m_TexturesGUI{};
    bool m_GUIRendering{false};
    //

 



    AssetManager* m_AssetManager{};

};
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageseverity,VkDebugUtilsMessageTypeFlagsEXT messagetype,const VkDebugUtilsMessengerCallbackDataEXT* pcallbackdata,void* puserData );
 