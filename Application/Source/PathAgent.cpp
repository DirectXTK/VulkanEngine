#include "PathAgent.h"
#include "Application.h"
PathAgent::PathAgent(const Float2& position,const Float2& size,const AgentType& type,PathSystem* system): m_Position(position),m_Size(size),m_Type(type),m_System(system){

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
std::shared_ptr<std::vector<Float2>> PathAgentHandle::GetPathToObj(const Float2& endPos){
    PathAgent* agent = m_System->GetAgent(m_Index);
    return m_System->GetPathToObj(agent->GetPosition(),endPos);
}
void PathSystem::   RenderGrid(){
    Renderer* renderer=Application::GetRenderer();

    for(uint32_t y=0;y <m_GridHeight;y++){

        for(uint32_t x=0;x < m_GridWidth;x++){

            if(m_Grid[(y*m_GridWidth)+x]== AgentType::LAND){
                renderer->DrawQuad({x*m_TileSize.x,y*m_TileSize.y,0.0f},{1.0f,0.0f,0.0f,1.0f},{m_TileSize.x*0.5f,m_TileSize.y*0.5f},0);
            }

        }
    }
}
void PathSystem::ResetGrid(){
    memset(m_Grid,0,sizeof(AgentType)*m_GridWidth*m_GridHeight);
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
void GenerateSuccessorNodes(Node* nodes,Node& parent,uint32_t width){

    nodes[0].PosX = parent.PosX-1;
    nodes[1].PosX = parent.PosX;
    nodes[2].PosX = parent.PosX+1;

    nodes[3].PosX = parent.PosX-1;
    nodes[4].PosX = parent.PosX+1;
    
    nodes[5].PosX = parent.PosX-1;
    nodes[6].PosX = parent.PosX;
    nodes[7].PosX = parent.PosX+1;

    nodes[0].PosY = parent.PosY+1;
    nodes[1].PosY = parent.PosY+1;
    nodes[2].PosY = parent.PosY+1;

    nodes[3].PosY = parent.PosY;
    nodes[4].PosY = parent.PosY;

    nodes[5].PosY = parent.PosY-1;
    nodes[6].PosY = parent.PosY-1;
    nodes[7].PosY = parent.PosY-1;

    
    for(uint32_t i=0 ;i < 8;i++){
      //  nodes[i].parent = &parent;
    }
}
float CalcDistance(int32_t firstX,int32_t firstY,int32_t secondX,int32_t secondY){
    int dx = abs(firstX - secondX);
    int dy = abs(firstY - secondY);
    return (dx + dy) + (1.4142f - 2) * std::min(dx, dy); 
}
bool IsValid(int posX,int posY,uint64_t width,uint64_t height){
    if(posX < 0 || posY < 0|| posX >= width||posY >= height)
        return false;
    return true;
}
struct NodeCompare{
     bool operator()(const Node* a, const Node* b) const {
        return a->f > b->f;
    } 
};
std::shared_ptr<std::vector<Float2>> PathSystem::GetPathToObj(const Float2& startPos,const Float2& endPos){
    uint32_t endX{},endY{};
    uint32_t startX{},startY{};
    endX = endPos.x/m_TileSize.x;
    endY = endPos.y/m_TileSize.y;

    startX = startPos.x/m_TileSize.x;
    startY = startPos.y/m_TileSize.y;

    if(startX == endX && startY == endY)
        return nullptr;
   
    std::priority_queue<Node*, std::vector<Node*>, NodeCompare> openSet;
    bool* closedSet = new bool[m_GridWidth*m_GridHeight];
    Node* nodeGrid = new Node[m_GridWidth*m_GridHeight];

    //std::shared_ptr<std::vector<Float2>> defRet = std::make_shared<std::vector<Float2>>(std::vector<Float2>());

     for(uint32_t i =0 ;i < m_GridWidth*m_GridHeight;i++){
        nodeGrid[i] = Node();
        closedSet[i] = false;
    }
    Node* start = &nodeGrid[startX+(startY*m_GridWidth)];
    start->g = 0.0f;
    start->h = 0.f;
    start->f = 0.f;

    start->PosX = startX;
    start->PosY = startY;


    openSet.push(start);

   
    while(!openSet.empty()){

        Node* parent = openSet.top();

        openSet.pop();
        closedSet[parent->PosX+(m_GridWidth*parent->PosY)] = true;

        Node nodes[8]{};
        GenerateSuccessorNodes(nodes,*parent,m_GridWidth);

        for(uint32_t i=0;i < 8;i++){
            float fNew{};
            float hNew{};
            float gNew{};
            Node* current{};
            
            if(!IsValid(nodes[i].PosX,nodes[i].PosY,m_GridWidth,m_GridHeight)){
                continue;
            }
            current = &nodeGrid[nodes[i].PosX+(nodes[i].PosY*m_GridWidth)];
            current->PosX = nodes[i].PosX;
            current->PosY = nodes[i].PosY;

            //current->parent = parent;


            
           // defRet.get()->push_back({current->PosX*m_TileSize.x,current->PosY*m_TileSize.y});

            if(current->PosX == endX&& current->PosY == endY){
                std::shared_ptr<std::vector<Float2>> ret = std::make_shared<std::vector<Float2>>(std::vector<Float2>());
                std::vector<Float2>& path = *ret.get();
                int posY = endY;
                int posX = endX;

                current->parent = parent;

              
                while (current != nullptr) {
                    path.push_back({current->PosX * m_TileSize.x,
                    current->PosY * m_TileSize.y});
                    current = current->parent;
                }
                std::reverse(path.begin(), path.end());
                delete[] closedSet;
                delete[] nodeGrid;
                return ret;
            }

            if(!closedSet[current->PosX+(current->PosY*m_GridWidth)]){
                gNew=parent->g+1.0f;
                hNew = CalcDistance(current->PosX,current->PosY,endX,endY);
                fNew = gNew+hNew;


                 if(current->f == FLT_MAX ||
               current->f > fNew     ){
                current->f = fNew;
                current->h = hNew;
                current->g = gNew;
                current->parent = parent;
                openSet.push(current);
            }
            }else{
            }

           
            

        }

         

         


    }

    delete[] closedSet;
    delete[] nodeGrid;
    return nullptr;
}
