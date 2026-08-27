#include "RendererFrontEnd.h"
#include "GUI.h"
#include "Application.h"
//
struct DrawData{
    Float2 pos{};
    Float2 size{};
    Float4 color{};
    GUUID id{0};
    int32_t textureIndex{-1};//TODO remove use index inside texture
    GUUID textureID{0};
};
struct DrawTextData{
    char* msg{};
    uint64_t msgLen{};
    Float2 pos{};
    Float2 boundingBox[4];
    float fixedPadding{};
    float charSizeNorm{};
    GUUID id{0};
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
    Float2 size{};
    Float4 color{};
    GUUID id{};
    int32_t textureIndex{-1};//TODO remove use index inside texture
    GUUID textureID{0};
    //Dynamic data
    std::vector<Float2> positions{};

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
void Render::DrawQuad(const Float2& pos,const Float4& color,const Float2&size,GUUID id,Animator animation){
     DrawQuad(pos,color,size,id,animation.GetCurrentTexture().GetID(),animation.GetTextureIndex());
}
void Render::DrawInstance(const Float2& pos,const Float4& color,const Float2& size,GUUID id,GUUID textureID ,int32_t textureIndex){
    CmdCommand cmd{};

    if(m_CommandBuffer[m_CurrentFrame].Size() !=0){
        CmdCommand lastCmd = m_CommandBuffer[m_CurrentFrame].Last();
        if(lastCmd.type == CmdTypes::DRAWINSTANCED){
            DrawDataInstanced* lastDrawData = (DrawDataInstanced*)lastCmd.data;
            if(lastDrawData->color == color && lastDrawData->size == size&& lastDrawData->textureID == textureID ){
                lastDrawData->positions.push_back(pos);
                return;
            }

        }
    }
    DrawDataInstanced* drawData = new DrawDataInstanced();
    drawData->color = color;
    drawData->size = size;
    drawData->id = id;
    drawData->positions.push_back(pos);
    drawData->textureID = textureID;
    drawData->textureIndex = textureIndex;
    //TODO complete the draw list

    cmd.data = drawData;
    cmd.type = CmdTypes::DRAWINSTANCED;
    m_CommandBuffer[m_CurrentFrame].AddCmd(cmd);
}
void Render::DrawInstance(const Float2& pos,const Float4& color,const Float2& size,GUUID id,Animator animation){
    DrawInstance(pos,color,size,id,animation.GetCurrentTexture().GetID(),animation.GetTextureIndex());
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
void Render::DrawQuad(const Float2& pos,const Float4& color,const Float2&size,GUUID id){
  
    DrawQuad(pos,color,size,id,GUUID(0),-1);

}
void Render::DrawQuad(const Float2& pos,const Float4& color,const Float2&size,GUUID id,GUUID textureID,int textureIndex){
    CmdCommand cmd{};
    DrawData* drawData = new DrawData();
    drawData->pos = {pos.x,pos.y};
    drawData->color = color;
    drawData->size = size;
    drawData->id = id;
    drawData->textureID = textureID;
    drawData->textureIndex = textureIndex;
    cmd.data = drawData;
    cmd.type = CmdTypes::DRAW;
    m_CommandBuffer[m_CurrentFrame].AddCmd(cmd);
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
            m_Renderer->DrawQuad({data->pos.x,data->pos.y,0.0f},data->color,data->size,data->textureID,data->id.ID,data->textureIndex);
            delete data;

            break;
        }
        case CmdTypes::DRAWTEXT:{
            DrawTextData* data=(DrawTextData*)cmd.data;
            m_Renderer->RenderText(data->msg,data->msgLen,data->pos,data->boundingBox,data->fixedPadding,0,data->id,data->pointerIndex);
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
		            m_Renderer->SetCurrentFont(data->fontAsset);
                }
            }else{
                std::string& name = m_Renderer->GetCurrentFont().GetData()->FontName;
                GUUID id = FONTID(name,data->charSize);
                Asset<Font> newFont = Application::GetAsset<Font>(id);
                if(newFont){
                    
		            m_Renderer->SetCurrentFont(newFont);

                }else{
                    Core::Log("Need to recreate");
                }


            }
        
            delete data;
            break;
        }
        case CmdTypes::DRAWINSTANCED:{
            DrawDataInstanced* data=(DrawDataInstanced*)cmd.data;
            for(uint32_t i=0;i < data->positions.size();i++)
                m_Renderer->DrawInstance(data->positions[i],data->color,data->size,data->id.ID,data->textureID,data->textureIndex);
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
if(!guiRender){
        m_Renderer->BeginGUIFrame();
        cmdBufferOffset = cmdBufferEnd;
        cmdBufferEnd= cmdB.Size();
        guiRender = true;
        goto Render;
    }
    m_CommandBufferGUIBeginIndex=0;
    cmdB.ResetBuffer();
}
void Render::StartQueue(Camera2D& camera){
    Application::UpdateContent(m_CurrentFrame);
    m_ViewProj[m_CurrentFrame] = camera.GetViewProj();
    m_CameraPos[m_CurrentFrame] = camera.GetPosition();
    m_CameraSize[m_CurrentFrame] = camera.GetScale();
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
