#pragma once
#include "AppCore.h"
#include "Renderer.h"
class Render{
    public:

    Render(Renderer* renderer,void* guiRenderer);
    void StartQueue(Camera2D& camera){m_ViewProj[m_CurrentFrame] = camera.GetViewProj();m_CameraPos[m_CurrentFrame] = camera.GetPosition(); m_CameraSize[m_CurrentFrame] = camera.GetScale();}
    void StartGUIQueue();
    
    void DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id,Animator animation);
    void DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id,GUUID textureID,int32_t textureIndex);
    void DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id);
    void DrawInstance(const Float2& pos,const Float4& color,const Float2& size,GUUID id,GUUID textureID ,int32_t textureIndex);
    void DrawInstance(const Float2& pos,const Float4& color,const Float2& size,GUUID id,Animator animation);
    
    void DrawText(const char* Message,uint64_t bufferSize, Float2 Position, Float2 BoundingBox[4], float FixedPadding,float CharSizePixels,GUUID id,int64_t PointerIndex); 

    void SetShader(Asset<Shader> asset);
    void RemoveShader(const ShaderType& shaderType);

    //if asset is not provided then font stays it only changes size
    void SetFont(Asset<Font> asset,uint32_t charSize);

    void FinishQueue();

    void FinishRenderGUI();
    void FinishRenderGeometry();

    void EndFrame();

    glm::mat4 GetViewProj(){return m_ViewProj[m_RenderedFrameIndex];}
    uint32_t GetRenderedFrame(){return m_RenderedFrameIndex;}
    uint32_t GetReadyFrameCount(){return m_ReadyFrames;}

    Float2 GetCameraSize(){return m_CameraSize[m_RenderedFrameIndex];}
    Float2 GetCameraPos(){return m_CameraPos[m_RenderedFrameIndex];}

    ~Render();

    private:

    void RunCommands();

    enum class CmdTypes{NONE,DRAW,DRAWTEXT,DRAWINSTANCED,CHANGESHADER,CHANGEFONT};
    struct CmdCommand{
        CmdTypes type{};
        void* data{};
    };
    class CmdBuffer{
        public:
        uint64_t Size(){return m_Commands.size();}
        CmdCommand& Current(){
            
                        #ifdef DEBUG
            if(m_CurrentCmdIndex >= m_Commands.size() )
            {
                Core::Log(ErrorType::Error,"Index is out of bounds CmdBuffer::Current()");
                CmdCommand cmd{};
                return cmd;
            }
            #endif
            
            
            return m_Commands[m_CurrentCmdIndex];
        }
        void Next(){   m_CurrentCmdIndex++; }
        void ResetBuffer(){
            m_CurrentCmdIndex=0;
            m_Commands.clear();
            m_Commands.resize(0);
        }
        void AddCmd(const CmdCommand& cmd){
            m_Commands.push_back(cmd);
        }
        private:
        uint64_t m_CurrentCmdIndex{0};
        std::vector<CmdCommand> m_Commands{};
    };
    struct RenderQueue{
        Float3 pos{};
        Float4 color{};
        Float2 size{};
        GUUID id{};
        GUUID textureID{0};
        int32_t textureId{-1};
        Animator animation{};
        ~RenderQueue()=default;
    };
    struct RenderQueueText{
        char* message{nullptr};
        uint64_t bufferSize{};  
        Float2 pos{};
        //Size 4 array
        Float2* boundingBox;
        float fixedPadding{};
        float charPixelSize{};
        GUUID id{};
        int64_t pointerIndex{};

    };
    struct InstanceQueue{
        Float2 pos{};
        Float4 color{};
        Float2 size{};
        GUUID id{};
        GUUID textureID{};
        int32_t textureIndex{-1};
    };
    Renderer* m_Renderer{};
    void* m_GUI{};

    uint32_t m_RenderedFrameIndex{};
    uint32_t m_CurrentFrame{};
    std::atomic<uint32_t> m_ReadyFrames{0};

    uint64_t m_CommandBufferGUIBeginIndex{0};//Indexes then GUI command start in the commandbuffer.
    CmdBuffer m_CommandBuffer[MAX_FRAME_DRAWS];

    uint32_t m_GeometryEndIndexes[MAX_FRAME_DRAWS];
    uint32_t m_GeometryEndIndexesText[MAX_FRAME_DRAWS];

    glm::mat4 m_ViewProj[MAX_FRAME_DRAWS];
    Float2 m_CameraSize[MAX_FRAME_DRAWS];
    Float2 m_CameraPos[MAX_FRAME_DRAWS];

    std::vector<ShaderType> m_RemovedShaders[MAX_FRAME_DRAWS];
    std::vector<Asset<Shader>> m_AddedShaders[MAX_FRAME_DRAWS];

    std::vector<InstanceQueue> m_InstanceQueue[MAX_FRAME_DRAWS];
    std::vector<RenderQueue> m_RenderQueues[MAX_FRAME_DRAWS];
    std::vector<RenderQueueText> m_RenderQueuesText[MAX_FRAME_DRAWS];
};