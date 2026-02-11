#include "PathAgent.h"
#include "Application.h"
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
float Area(const Float2& a, const Float2& b, const Float2& c) {
    return (b.x - a.x) * (c.y - a.y) -
           (b.y - a.y) * (c.x - a.x);
}
bool PointInTri(
    const Float2& p,
    const Float2& a,
    const Float2& b,
    const Float2& c)
{
    float a1 = Area(p, a, b);
    float a2 = Area(p, b, c);
    float a3 = Area(p, c, a);
    return (a1 >= 0 && a2 >= 0 && a3 >= 0) ||
           (a1 <= 0 && a2 <= 0 && a3 <= 0);
}
std::vector<std::array<int,3>> Triangulate(const std::vector<Float2>& verts)
{
    std::vector<std::array<int,3>> tris;
    std::vector<int> idx(verts.size());

    for (int i = 0; i < verts.size(); i++)
        idx[i] = i;

    while (idx.size() > 3) {
        for (int i = 0; i < idx.size(); i++) {
            int p = idx[(i - 1 + idx.size()) % idx.size()];
            int c = idx[i];
            int n = idx[(i + 1) % idx.size()];

            if (Area(verts[p], verts[c], verts[n]) <= 0)
                continue; // not convex

            bool ear = true;
            for (int j : idx) {
                if (j == p || j == c || j == n) continue;
                if (PointInTri(verts[j],
                               verts[p],
                               verts[c],
                               verts[n])) {
                    ear = false;
                    break;
                }
            }

            if (ear) {
                tris.push_back({p, c, n});
                idx.erase(idx.begin() + i);
                break;
            }
        }
    }

    tris.push_back({idx[0], idx[1], idx[2]});
    return tris;
}
std::vector<NavPoly> BuildNavMesh2D(const std::vector<Float2>& boundary)
{
    auto tris = Triangulate(boundary);

    std::vector<NavPoly> mesh;
    for (auto& t : tris) {
        NavPoly p;
        p.verts = {
            boundary[t[0]],
            boundary[t[1]],
            boundary[t[2]]
        };
        mesh.push_back(p);
    }

    // Adjacency via shared edges
    for (int i = 0; i < mesh.size(); i++) {
        for (int j = i + 1; j < mesh.size(); j++) {
            int shared = 0;
            for (auto& a : mesh[i].verts)
                for (auto& b : mesh[j].verts)
                    if (a.x == b.x && a.y == b.y)
                        shared++;

            if (shared == 2) {
                mesh[i].neighbors.push_back(j);
                mesh[j].neighbors.push_back(i);
            }
        }
    }

    return mesh;
}
struct Portal {
    Float2 left;
    Float2 right;
};

Portal FindPortal(const NavPoly& a, const NavPoly& b)
{
    Portal p;
    int found = 0;

    for (const auto& va : a.verts) {
        for (const auto& vb : b.verts) {
            if (va.x == vb.x && va.y == vb.y) {
                if (found == 0) p.left = va;
                else            p.right = va;
                found++;
            }
        }
    }

    return p;
}
std::vector<Portal> BuildPortals( const std::vector<int>& polyPath, const std::vector<NavPoly>& navMesh)
{
    std::vector<Portal> portals;
    for (int i = 0; i < polyPath.size() - 1; i++) {
        portals.push_back(
            FindPortal(
                navMesh[polyPath[i]],
                navMesh[polyPath[i + 1]]
            )
        );
    }

    return portals;
}
std::vector<Float2> Funnel(const std::vector<Portal>& portals)
{
    std::vector<Float2> result;
    result.push_back(portals[0].left);

    Float2 apex  = portals[0].left;
    Float2 left  = portals[0].left;
    Float2 right = portals[0].right;

    int apexIdx = 0, leftIdx = 0, rightIdx = 0;

    for (int i = 1; i < portals.size(); i++) {
        const Float2& newLeft  = portals[i].left;
        const Float2& newRight = portals[i].right;

        // Update right
        if (Area(apex, right, newRight) <= 0) {
            if (apex == right || Area(apex, left, newRight) > 0) {
                right = newRight;
                rightIdx = i;
            } else {
                result.push_back(left);
                apex = left;
                apexIdx = leftIdx;
                left = apex;
                right = apex;
                leftIdx = apexIdx;
                rightIdx = apexIdx;
                i = apexIdx;
                continue;
            }
        }

        // Update left
        if (Area(apex, left, newLeft) >= 0) {
            if (apex == left || Area(apex, right, newLeft) < 0) {
                left = newLeft;
                leftIdx = i;
            } else {
                result.push_back(right);
                apex = right;
                apexIdx = rightIdx;
                left = apex;
                right = apex;
                leftIdx = apexIdx;
                rightIdx = apexIdx;
                i = apexIdx;
                continue;
            }
        }
    }

    result.push_back(portals.back().left);
    return result;
}
int FindContainingPolygon(const Float2& pos){
    return 1;
}
float CalculateDistanceFloat(const Float2& first,const Float2& second){
    return std::fabs(first.x-second.x)+std::fabs(first.y-second.y);
}
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

    std::vector<Float2> walkable{{0.0f,0.0f},{0.5f,0.0f},{1.0f,0.0},{1.0f,0.5f},{1.0f,1.0f},{0.5f,1.0f},{0.0f,1.0f},{0.0f,0.5f},{0.0f,0.4f},{0.0f,0.1f}};

    m_NavMesh =BuildNavMesh2D(walkable);

}
std::vector<Float2> PathAgentHandle::GetPathToObj(const Float2& goalPos){
    PathAgent* agent = m_System->GetAgent(m_Index);
    int endPoly= m_System->GetPolygonIndex(goalPos);
    int startPoly= m_System->GetPolygonIndex(agent->GetPosition());
    if(endPoly ==-1|| startPoly == -1)
        return {};
    std::vector<Float2> res = m_System->GetPathToObj(startPoly,endPoly);
    res.push_back(goalPos);
    return res;
}
void PathSystem::RenderGrid(){
    Renderer* renderer=Application::GetRenderer();

    for(uint32_t y=0;y <m_GridHeight;y++){

        for(uint32_t x=0;x < m_GridWidth;x++){

            if(m_Grid[(y*m_GridWidth)+x]== AgentType::LAND){
                renderer->DrawQuad({x*m_TileSize.x,y*m_TileSize.y,0.0f},{1.0f,0.0f,0.0f,1.0f},{m_TileSize.x*0.5f,m_TileSize.y*0.5f},0);
            }

        }
    }
}
int PathSystem::GetPolygonIndex(const Float2& pos){
    for(uint32_t i =0 ;i  < m_NavMesh.size();i++){
        if(PointInTri(pos,m_NavMesh[i].verts[0],m_NavMesh[i].verts[1],m_NavMesh[i].verts[2])){
            return i;
        }
    }
    return -1;
}
void PathSystem::ResetGrid(){
    memset(m_Grid,0,sizeof(AgentType)*m_GridWidth*m_GridHeight);
}
void PathSystem::Update(const Float2& position,const Float2& size,const AgentType& type){
    uint32_t y = position.y/m_TileSize.y;
    uint32_t x = position.x/m_TileSize.x;
    //m_Grid[(y*m_GridWidth)+x] = type;
}
PathAgentHandle PathSystem::CreateAgent(const Float2& position,const Float2& size,const AgentType& type){
    m_Agents.emplace_back(position,size,type,this);
    uint32_t x = position.x/m_TileSize.x;
    uint32_t y = position.y/m_TileSize.y;
    m_Grid[(y*m_GridWidth)+x] = type;


    return {(uint32_t)m_Agents.size()-1,this};
}
Float2 operator+(Float2 a, Float2 b){ return {a.x+b.x, a.y+b.y}; }
Float2 operator-(Float2 a, Float2 b){ return {a.x-b.x, a.y-b.y}; }
Float2 operator*(Float2 a, float s){ return {a.x*s, a.y*s}; }


float Dot(Float2 a, Float2 b){ return a.x*b.x + a.y*b.y; }
float Length(Float2 v){ return std::sqrt(Dot(v,v)); }
Float2 Normalize(Float2 v){
    float l = Length(v);
    return l > 0 ? v * (1.0f / l) : Float2{0,0};
}
Float2 DesiredVelocity(const Float2& pos,const Float2& target,float speed)
{
    return Normalize(target - pos) * speed;
}
Float2 ClampLength(Float2 v, float maxLen) {
    float len = Length(v);
    if (len > maxLen)
        return Normalize(v) * maxLen;
    return v;
}
Float2 Perp(const Float2& v) {
    return Float2{-v.y, v.x};
}
Float2 PathAgentHandle::MoveObject(const Float2& speed,const Float2& target){
      float lookAhead = 5 * m_System->GetAgent(m_Index)->GetSize().x;
    Float2 force{0,0};
    Float2 pos = m_System->GetAgent(m_Index)->GetPosition();
    Float2 desiredDir = Normalize(target - pos);

    for(uint32_t i=0; i<m_System->GetAgentCount(); i++){
        if(i == m_Index) continue;

        PathAgent& o = *m_System->GetAgent(i);
        Float2 toObs = o.GetPosition() - pos;
        float dist = Length(toObs);
        float radius = o.GetSize().x;

        if(dist > lookAhead + radius) continue;
        if(Dot(desiredDir, Normalize(toObs)) < 0) continue;

        float strength = std::min((lookAhead + radius - dist)/lookAhead, 1.0f);
        Float2 dir = Normalize(pos - o.GetPosition());

        float forwardComp = Dot(dir, desiredDir);
        float sideComp = Dot(dir, Perp(desiredDir));

        // Slow down forward if obstacle very close
        float forwardMultiplier = 1.0f;
        if(forwardComp > 0 && dist < radius + 0.5f*lookAhead){
            forwardMultiplier = dist / (radius + 0.5f*lookAhead);
        }

        Float2 avoidVec = desiredDir * forwardComp * forwardMultiplier + Perp(desiredDir) * sideComp;
        force += avoidVec * strength;
    }

    force = ClampLength(force, 1.0f);
    Float2 finalDir = Normalize(desiredDir + force);
    return finalDir * speed.x;
}


std::vector<Float2> PathSystem::GetPathToObj(int startPoly,int endPoly){

    std::priority_queue<PolyNode, std::vector<PolyNode>, std::greater<PolyNode>> openSet{};
    std::unordered_map<int,float> gScore{};
    std::unordered_map<int,int> cameFrom{};

    gScore[startPoly] = 0.0f;
    openSet.push({startPoly,0.0f});

    if(startPoly == endPoly){
        std::vector<Float2> path{};

        path.push_back(m_NavMesh[startPoly].center);

        return path;
    }

    while(!openSet.empty()){
        int current = openSet.top().poly;
        openSet.pop();

        if(current == endPoly){

            std::vector<int> path{};
            while(current != startPoly){
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(startPoly);
            std::reverse(path.begin(),path.end());
            return Funnel(BuildPortals(path,m_NavMesh));

        }

        NavPoly& poly = m_NavMesh[current];

        for(int neighbour : poly.neighbors){
            float tentG = gScore[current]+CalculateDistanceFloat(poly.center,m_NavMesh[neighbour].center);

            if(!gScore.count(neighbour) || tentG < gScore[neighbour]){
                cameFrom[neighbour] = current;
                gScore[neighbour] = tentG;

                float h = CalculateDistanceFloat(m_NavMesh[neighbour].center,m_NavMesh[endPoly].center);

                openSet.push({neighbour,tentG+h});
            }

        }
           
            


         

         


    }
    return {};
}



/*
std::shared_ptr<std::vector<Float2>> PathSystem::GetPathToObjOLD(const Float2& startPos,const Float2& endPos){
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
*/