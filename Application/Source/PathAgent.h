#pragma once
#include "AppCore.h"
enum class AgentType{NONE,WALKABLE,LAND,FLYING,BURROWED};
class PathSystem;


struct Node{
    int32_t PosX{};
    int32_t PosY{};

    float f{FLT_MAX};
    float h{FLT_MAX};
    float g{FLT_MAX};
    Node* parent{};

    bool operator<(const Node& other)const{
        return f < other.f;
    }
};
class PathAgent{
public:
    PathAgent(const Float2& position,const Float2& size,const AgentType& type,PathSystem* system);
    std::shared_ptr<std::vector<Float2>> GetPathToObj(const Float2& endPos);
    void Update(const Float2& position,const Float2& size);

    AgentType GetType(){return m_Type;}
    Float2& GetPosition(){return m_Position;}
    Float2& GetSize(){return m_Size;}

private:
 AgentType m_Type{};
 Float2 m_Position{};
 Float2 m_Size{};

 PathSystem* m_System{};
};
class PathAgentHandle{
    public:
    PathAgentHandle(){}
    PathAgentHandle(uint32_t index,PathSystem* system): m_Index(index),m_System(system){}

    std::shared_ptr<std::vector<Float2>> GetPathToObj(const Float2& endPos);
    void Update(const Float2& position,const Float2& size);

    private:
    uint32_t m_Index{};
    PathSystem* m_System{};
};
class PathSystem{
public:
    PathSystem(uint32_t gridWidth,uint32_t gridHeight,const Float2& oneTileSize);
    PathAgentHandle CreateAgent(const Float2& position,const Float2& size,const AgentType& type);
    void Update(const Float2& position,const Float2& size,const AgentType& type);

    void RenderGrid();

    void ResetGrid();

    protected:
     friend PathAgentHandle;
     PathAgent* GetAgent(uint32_t index){return &m_Agents[index];}
     std::shared_ptr<std::vector<Float2>> GetPathToObj(const Float2& startPos,const Float2& endPos);
    private:

    std::vector<PathAgent> m_Agents{};
    uint32_t m_GridWidth{};
    uint32_t m_GridHeight{};
    Float2 m_TileSize{};
    AgentType* m_Grid{};
    
};