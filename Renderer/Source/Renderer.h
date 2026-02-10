#pragma once
#define DEMO_FEATURES
#ifdef DEMO_FEATURES
#include "GUI.h"
#endif

#include "Pipeline.h"
#include "SwapChain.h"
#include "Shader.h"
#include "CommandPool.h"
#include "FrameBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Descriptor.h"
#include "Camera.h"
#include "Buffer.h"
#include "VulkanInstance.h"
#include "Texture.h"
#include "Animator.h"
#include "AssetManager.h"
#include "FontSystem.h"

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
    uint32_t VertexBufferIndex{};
    uint64_t VertexBufferOffset{};
    uint32_t DescriptorSetTextureIndex{};
};
enum class RenderMode{SOLID,WIREFRAME};

struct RendererDesc{
    uint32_t VertexCountPerDrawCall{100};
    Float4 ClearColor{};
    RenderMode Rendermode{RenderMode::SOLID};
    bool Blending{true};
    Camera2D* InitialCamera{nullptr}; //optional
    VkViewport Viewport{};
    Shader* Shaders{};
};
struct UniformCameraBufferData{
    glm::mat4 GeometryCamera{};
    glm::mat4 GUICamera{};
};
class Renderer {
public:
    //Initialization functions
    Renderer(RendererDesc desc, GLFWwindow* window, InputSystem* inputsystem, AssetManager* assetManager);
    void InitializePipeline(uint64_t MaxTextureCount);
    //
    void BeginFrame(Camera2D* camera,float deltaTime);
    void BeginGUIFrame();


    //
    void DrawQuad(Float3 Position, Float4 Color, Float2 Size, GUUID TextureHandle, uint64_t ID, int TextureIndex = -1);
    //void DrawQuadWithAtlas(Float3 Position, Float4 Color, Float2 Size, GUUID textureatlas, uint64_t ID, uint64_t TextureIndex);
    void DrawQuad(Float3 Position, Float4 Color, Float2 Size, Animator Animation, uint64_t ID);

    void DrawQuad(Float3 Position, Float4 Color, Float2 Size, uint64_t ID);

    void SetCurrentFont(Asset<Font> CurrentAsset);
    Asset<Font> GetCurrentFont(){return m_CurrentFont;}
    //PointerIndex = -1 means don't draw it.
    void RenderText(const char* Message, Float2 Position, Float2 BoundingBox[4], float FixedPadding,float CharSizeNorm,GUUID id,int64_t PointerIndex=-1);
    //GUI 
    //Particles
    void DrawParticle();

    void SetRenderDesc(const RendererDesc& desc);
    void QueueShaderChange(const std::string& path);
    void QueueShaderChange(Asset<Shader> shaderAsset);


    Buffer* GetCustomBuffer(uint32_t index) { return m_PickingImageBuffer; }
    Buffer* GetViewportWithID();

    VkExtent2D GetViewPortExtent() { return m_SwapChain->GetExtent(); }


    void EndFrame();
    //Runs alls the shaders changes pipeline changes when rendering is finished.
    void RunRendererChangeQueue();

    void OnWindowResize(uint32_t width,uint32_t height);

    void Statistics(bool renderGui = true,void* guirenderer= nullptr);

    Texture* LoadTexture(std::string Path,TextureType = TextureType::Texture);

    Context GetContext() { return m_Context; }
    VkResult GetSwapChainState(){return m_AcquireImageResult;}

    void FinishExecution();
    void Shutdown();
    float GetFONTDPI();
    void ChangeArrowColor(const Float4& color){m_ArrowColor = color;}

    ~Renderer();
private:
    void InitRenderDesc(const RendererDesc& desc);
    void ResizeWindow();
    bool CompileShaders();

    void ResetFrameData();

    void ReCreatePipeline(const PipelineDesc& desc);

    void StartRecordingCommands();
    void StopRecordingCommands();

    void DrawBatch();
    void DrawGUIBatch();


    void CreateNewBufferForBatch(std::vector<Buffer*>& VertexBuffers, std::vector<Buffer*>& Stagging);

    void FlushGeometry();
    void FlushGUI();

    void CreateInstance();
    void CreateColorAttachments(uint32_t Count,std::vector<Texture*>& colorAttachments,const TextureCreateInfo& createInfo);
    void CreateFrameBuffers();
    void ReCreateFrameBuffers();

    void CreateCommandBuffers();
    void CreateDescriptorSets();

    void CreatePickingImage();
    //Options
    RendererDesc m_RendererDesc{};
    RendererDesc m_RendererDescNext{};
    PipelineDesc m_PipelineDesc{};

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
    bool m_ResizeWindow{};
    Float2 m_NewWindowSize{};

    SwapChain* m_SwapChain{};
    VkResult m_AcquireImageResult{};

    uint32_t m_ImageIndex{};
    std::vector<VkCommandBuffer> m_CommandBuffers{};
    std::vector<VkCommandBuffer>  m_TransferCommandBuffers{};
    //Used for transfering files
    std::array<std::vector<Buffer*>,MAX_FRAME_DRAWS> m_TempBuffers{};
    VkCommandBuffer m_CurrentCommandBuffer{};

    std::vector<FrameBuffer> m_FrameBuffers{};
    std::vector<Texture*> m_ColorAttachments{};
    //
    std::vector<Texture*> m_DepthStencilAttachments{};

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
    std::vector<VkFence> m_ImageFreeF{};
    //Buffers

    std::vector<Buffer*> m_IndexBuffers{};
    uint32_t* m_Indices{};
    


    std::array<Buffer*,MAX_FRAME_DRAWS> m_UniformBuffer{};
    //Descriptors 
    DescriptorPool m_DescriptorPool{};

    std::array<DescriptorSet,MAX_FRAME_DRAWS> m_DescriptorSetCamera{};

    //Debug
    VkDebugUtilsMessengerEXT m_Messenger{};
    //all the rendered vertecies of the frame.
    uint64_t m_VertexCountPerFrame{};
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
    uint32_t m_VertexCountRemaining{};
    uint64_t m_VertexBufferOffset{};
    uint32_t m_CurrentVertexBufferIndex{};
    uint64_t m_VertexCountPerDrawCall{};

    float m_DeltaTime{};

    Texture* m_FrameImageIndexed{};
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

    uint32_t m_CurrentTextureDescriptorSetOffset{1};
    uint32_t m_CurrentCameraDescriptorSetOffset{};
    //Camera
    UniformCameraBufferData m_UniformCameraData{};

    //Text
    Asset<Font> m_CurrentFont{};




    Camera2D m_Camera{};
    //Texturing
    uint32_t m_TextureSlotCount{ 4};
    Texture* m_BlankWhiteTexture{};
    DescriptorPool m_DescriptorPoolTextures{};

    std::array<DescriptorSet,MAX_FRAME_DRAWS> m_DescriptorSetTextures{};

    struct TextureRenderingData {
        Asset<Texture> texture{};
        uint32_t Index{};
    };
    std::unordered_map<GUUID, TextureRenderingData> m_Textures[MAX_FRAME_DRAWS];
    std::vector<GUUID> m_TextureIDByOrder[MAX_FRAME_DRAWS];
    //GUI stuff
    Vertex* m_VerticesGUI{};
    uint64_t m_VertexMaxCountGUI{ 40 };
    uint64_t m_VertexCountGUI{};
    uint64_t m_VertexGUIRemaining{};
    uint64_t m_VertexPointerGUI{};
    uint64_t m_VertexBufferOffsetGUI{};
    uint32_t m_CurrentVertexBufferIndexGUI{};
    std::vector<DescriptorSet> m_DescriptorSetTexturesGUI{};
    bool m_GUIRendering{false};
    Float4 m_ArrowColor{1.0f,1.0f,1.0f,1.0f};
    //

    //Shutdown
    bool m_ShutDown{false};
    //Queue changes
    std::vector<Shader> m_QueuedShaders{};


    AssetManager* m_AssetManager{}; 
    //Shaders
    std::vector<std::string> m_LoadedShaderPaths{};   
    std::vector<std::string> m_QueuedShaderPaths{};   


};
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT messageseverity,VkDebugUtilsMessageTypeFlagsEXT messagetype,const VkDebugUtilsMessengerCallbackDataEXT* pcallbackdata,void* puserData );
 