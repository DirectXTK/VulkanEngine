#include "PathAgent.h"
#include "Application.h"
PathAgent::PathAgent(const Float2& position,const Float2& size,const AgentType& type,PathSystem* system): m_Position(position),m_Size(size),m_Type(type),m_System(system){

}

std::shared_ptr<Float2*> PathAgent::GetPathToObj(const Float2& endPos){




}
void PathAgent::Update(const Float2& position,const Float2& size){
    m_Position = position;
    m_Size  = size;
    m_System->Update(m_Position,m_Size,m_Type);
}

    void PathAgentHandle::Update(const Float2& position,const Float2& size){
            PathAgent* agent =  m_System->GetAgent(m_Index);
            agent->Update(position,size);
    }



PathSystem::PathSystem(uint32_t gridWidth,uint32_t gridHeight,const Float2& oneTileSize){
    m_TileSize = oneTileSize;
    m_GridWidth = gridWidth;
    m_GridHeight = gridHeight;
    m_Grid = new AgentType[m_GridWidth*m_GridHeight];
}
void PathSystem::RenderGrid(){
    Renderer* renderer=Application::GetRenderer();

    for(uint32_t y=0;y <m_GridHeight;y++){

        for(uint32_t x=0;x < m_GridWidth;x++){

            if(m_Grid[(y*m_GridWidth)+x]== AgentType::LAND){
                renderer->DrawQuad({x*m_TileSize.x,y*m_TileSize.y,0.0f},{1.0f,0.0f,0.0f,1.0f},m_TileSize,0);
            }

        }
    }
}
void PathSystem::Update(const Float2& position,const Float2& size,const AgentType& type){
    uint32_t y = position.y/m_TileSize.y;
    uint32_t x = position.x/m_TileSize.x;
    m_Grid[(y*m_GridWidth)+x] = type;
}
PathAgentHandle PathSystem::CreateAgent(const Float2& position,const Float2& size,const AgentType& type){
    m_Agents.emplace_back(position,size,type,this);
    uint32_t x = position.x/m_TileSize.x;
    uint32_t y = position.y/m_TileSize.y;
    m_Grid[(y*m_GridWidth)+x] = type;


    return {(uint32_t)m_Agents.size()-1,this};
}