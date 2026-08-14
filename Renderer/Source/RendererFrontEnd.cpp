#include "RendererFrontEnd.h"
#include "GUI.h"
#include "Application.h"
//
struct DrawData{
    Float2 pos{};
    Float2 size{};
    Float4 color{};
    GUUID id{};
    uint32_t textureIndex{};//TODO remove use index inside texture
    Asset<Texture> textureAsset{};
};
struct DrawTextData{
    char* msg{};
    uint64_t msgLen{};
    Float2 pos{};
    Float2 boundingBox[4];
    float fixedPadding{};
    float charSizeNorm{};
    GUUID id{};
    int64_t pointerIndex{};
    Float4 color{};
    ~DrawTextData(){
        delete msg;
    }
};
struct CmdChangeFontData{
    Asset<Font> fontAsset{};
    uint32_t charSize{};
    
};
struct DrawDataInstanced{
    uint32_t m_InstanceCount{};

};
Render::Render(Renderer* renderer,void* guiRenderer): m_Renderer(renderer),m_GUI(guiRenderer){
}
void Render::FinishQueue(){
    if(m_ReadyFrames == MAX_FRAME_DRAWS){
       return;
    }
    m_CurrentFrame++;
    m_ReadyFrames++;    
 
    if(m_CurrentFrame == MAX_FRAME_DRAWS){
        m_CurrentFrame =0;
    }

}
void Render::RemoveShader(const ShaderType& shaderType){
    m_RemovedShaders[m_CurrentFrame].push_back(shaderType);
}

void Render::SetShader(Asset<Shader> asset){
    m_AddedShaders[m_CurrentFrame].push_back(asset);
}
void Render::StartGUIQueue(){
   m_CommandBufferGUIBeginIndex = m_CommandBuffer[m_CurrentFrame].Size();
}
void Render::DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id,Animator animation){
    Core::Log(ErrorType::Warning,"Not implemented");

}
void Render::DrawInstance(const Float2& pos,const Float4& color,const Float2& size,GUUID id,GUUID textureID ,int32_t textureIndex){
    Core::Log(ErrorType::Warning,"Not implemented");

}
void Render::DrawInstance(const Float2& pos,const Float4& color,const Float2& size,GUUID id,Animator animation){
    Core::Log(ErrorType::Warning,"Not implemented");
}
void Render::DrawText(const char* Message,uint64_t bufferSize, Float2 Position, Float2 BoundingBox[4], float FixedPadding,float CharSizePixels,GUUID id,int64_t PointerIndex){
    char* msg = new char[strlen(Message)*sizeof(char)];
    strcpy(msg,Message);
    DrawTextData* data= new DrawTextData();
    data->msg = msg;
    data->msgLen = bufferSize;
    data->pos = Position;
    data->boundingBox[0]= BoundingBox[0];
    data->boundingBox[1]= BoundingBox[1];
    data->boundingBox[2]= BoundingBox[2];
    data->boundingBox[3]= BoundingBox[3];
    data->fixedPadding = FixedPadding;
    data->charSizeNorm = CharSizePixels;
    data->id = id;
    data->pointerIndex = PointerIndex;
    
    CmdCommand cmd{};
    cmd.data = data;
    cmd.type = CmdTypes::DRAWTEXT;
    m_CommandBuffer[m_CurrentFrame].AddCmd(cmd);
}
void Render::DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id){
    CmdCommand cmd{};
    DrawData* drawData = new DrawData();
    drawData->pos = {pos.x,pos.y};
    drawData->color = color;
    drawData->size = size;
    drawData->id;
    cmd.data = drawData;
    cmd.type = CmdTypes::DRAW;
    m_CommandBuffer[m_CurrentFrame].AddCmd(cmd);
}
void Render::DrawQuad(const Float3& pos,const Float4& color,const Float2&size,GUUID id,GUUID textureID,int textureIndex){
           Core::Log(ErrorType::Warning,"Not implemented");
}   
void Render::RunCommands(){
    CmdBuffer& cmdB = m_CommandBuffer[m_RenderedFrameIndex];
    GUIRenderer* gui = (GUIRenderer*)m_GUI;
    bool guiRender{false};
    uint64_t cmdBufferEnd =m_CommandBufferGUIBeginIndex;
    uint64_t cmdBufferOffset =0;
    //make two versions one with switch other with array and index to functions.
    //The geometry commads go uptil gui commands

Render:
    for(uint32_t i=cmdBufferOffset;i < cmdBufferEnd;i++){
        CmdCommand& cmd = cmdB.Current();
        cmdB.Next();
    switch(cmd.type){
        case CmdTypes::DRAW:{
            DrawData* data=(DrawData*)cmd.data;
            m_Renderer->DrawQuad({data->pos.x,data->pos.y,0.0f},data->color,data->size,data->id.ID);
            delete data;

            break;
        }
        case CmdTypes::DRAWTEXT:{
            DrawTextData* data=(DrawTextData*)cmd.data;
            m_Renderer->RenderText(data->msg,data->msgLen,data->pos,data->boundingBox,data->fixedPadding,data->charSizeNorm,data->id,data->pointerIndex);
            delete data;
            break;
        }
        case CmdTypes::CHANGEFONT:{
            CmdChangeFontData* data = (CmdChangeFontData*)cmd.data;
            if(data->fontAsset){
                FontSystem* m_FontSystem{};
                if(!Application::HasAsset(data->fontAsset.GetID())){
                    Core::Log("Invalid asset Render::RunCommands");

                }else{
                    
                }
            }else{
                    Application::GetApplication()->m_FontSystem->SetCharcterSize(data->charSize);
                   gui->SetFontSize(data->charSize);

            }
        
            delete data;
            break;
        }
       
        
       
        default:{
            Core::Log(ErrorType::Error,"Not implemented command or invalid {Render::RunCommands} type:",(int64_t)cmd.type);
            break;
        }
    }   
    }
end:
    m_Renderer->BeginGUIFrame();
    if(!guiRender){
        cmdBufferOffset = cmdBufferEnd;
        cmdBufferEnd= cmdB.Size();
        guiRender = true;
        goto Render;
    }
    m_CommandBufferGUIBeginIndex=0;
    cmdB.ResetBuffer();
}
void Render::EndFrame(){

    RunCommands();
    
   
    m_GeometryEndIndexes[m_RenderedFrameIndex] =0;
    m_GeometryEndIndexesText[m_RenderedFrameIndex] =0;

    m_RenderedFrameIndex++;
    if(m_RenderedFrameIndex == MAX_FRAME_DRAWS){
        m_RenderedFrameIndex =0;
    }   
    m_ReadyFrames--;
}
void Render::FinishRenderGUI(){

}

void Render::SetFont(Asset<Font> asset,uint32_t charSize){
    CmdChangeFontData* fontData = new CmdChangeFontData();
    fontData->fontAsset = asset;
    fontData->charSize = charSize;
    CmdCommand cmd{};
    cmd.type = CmdTypes::CHANGEFONT;
    cmd.data = fontData;
    m_CommandBuffer[m_CurrentFrame].AddCmd(cmd);
}
void Render::FinishRenderGeometry(){

}
Render::~Render(){
}
