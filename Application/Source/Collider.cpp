#include "Collider.h"
#define TILESIZE 0.04f


float CalculateDistance(float First, float Second) {
	return std::fabs(First - Second);
}


Collider::Collider(CollisionSystem* system,GUUID ID):m_System(system),m_ID(ID)
{
}

void Collider::Update(Float2* Position, Float2* Size)
{
	m_System->GetColliderBackEnd(m_ID)->Update(Position, Size);
}

void Collider::UpdateMoveAmount(Float2 MoveAmount)
{
	m_System->GetColliderBackEnd(m_ID)->UpdateMoveAmount(MoveAmount);
}

bool Collider::IsCollided()
{
	return m_System->GetColliderBackEnd(m_ID)->IsColided(); 
}
bool IsDestination(Float2 Destination,Float2 NodePos, ColliderBackEnd* collider) {
	float DistanceX = CalculateDistance(Destination.x, NodePos.x);
	float DistanceY = CalculateDistance(Destination.y, NodePos.y);

	if (DistanceX <= collider->GetSize().x && DistanceY <= collider->GetSize().y)
		return true;
	return false;
}
void GenerateSuccesors(Float2 Parent,Float2* succ,Float2 Size) {
	//middle
	succ[0].x = Parent.x  ;
	succ[1].x = Parent.x ;

	succ[0].y = Parent.y +Size.y;
	succ[1].y = Parent.y - Size.y;
	//Left
	succ[2].x = Parent.x - Size.x;
	succ[3].x = Parent.x - Size.x;
	succ[4].x = Parent.x - Size.x;

	succ[2].y = Parent.y + Size.y;
	succ[3].y = Parent.y ;
	succ[4].y = Parent.y -Size.y;
	//Right
	succ[5].x = Parent.x + Size.x;
	succ[6].x = Parent.x + Size.x;
	succ[7].x = Parent.x + Size.x;

	succ[5].y = Parent.y + Size.y;
	succ[6].y = Parent.y  ;
	succ[7].y = Parent.y - Size.y;

}
float CalculateHValue(Float2 CurrentNode, Float2 Destination) {
	return std::fabs(CurrentNode.y - Destination.y) + std::fabs(CurrentNode.x - Destination.x);
}
bool Collider::IsBlocked(ColliderBackEnd* movingCollider,Float2 Position) {
	return m_System->IsBlocked(movingCollider, Position);
}
Float2* TraceDest(Node* Details, uint64_t DestIndex,uint32_t* outDestCount) {
	std::stack<Node> Path;
	Float2* Output{};

	while (Details[DestIndex].ParentNodeIndex != DestIndex) {
		Node node;
		node.Position = Details[DestIndex].Position;
		Path.push(node);
		DestIndex = Details[DestIndex].ParentNodeIndex;
	}
	*outDestCount = Path.size();
	Output = new Float2[*outDestCount];

	uint32_t Index{};
	int32_t CurrentNodeX;
	while (!Path.empty()) {
		Output[Index] = Path.top().Position;
		Path.pop();
		//Core::Log(ErrorType::Info, Output[Index].x, " ", Output[Index].y);
			Index++;
	}
	
	

	return Output;
}
Float2* Collider::GetPathToObj(Float2 CurrentPos, Float2 FinalPath,uint32_t *outDestCount)
{
	//Generate succesors the size of the moving collider
	ColliderBackEnd* CurrentCollider = m_System->GetColliderBackEnd(m_ID);
	std::set<Node> OpenList{};
	std::unordered_map<Float2, uint32_t> ClosedList{};
	Float2 succ[8];
	uint64_t Index{};




	std::vector<Node> NodeDetails{};
	NodeDetails.resize(8000);

	NodeDetails.push_back(Node());

	NodeDetails[0].Position = CurrentPos;
	NodeDetails[0].F = 0.0f;
	NodeDetails[0].G = 0.0f;
	NodeDetails[0].H = 0.0f;
	NodeDetails[0].ParentNodeIndex = 0;

	OpenList.insert(NodeDetails[0]);


	while (!OpenList.empty()) {

		float gNew, fNew, hNew{};
		Node p = *OpenList.begin();
		ClosedList[p.Position] = 1;
		Index++;
		OpenList.erase(OpenList.begin());

		GenerateSuccesors(p.Position, succ, CurrentCollider->GetSize());
		for (uint32_t i = 0; i < 8; i++) {
			if (IsBlocked(CurrentCollider, succ[i]))
				continue;
			if (IsDestination(FinalPath,succ[i], CurrentCollider))
			{
				Core::Log(ErrorType::Error, "Found the path");
				
				NodeDetails[Index+i].Position = FinalPath;

				NodeDetails[Index + i].ParentNodeIndex = p.Index;

				return TraceDest(NodeDetails.data(),Index+i, outDestCount);
			}
			else if(ClosedList.find(succ[i]) ==ClosedList.end()) {
				gNew = NodeDetails[p.ParentNodeIndex].G + CurrentCollider->GetSize().x;
				hNew = CalculateHValue(succ[i], FinalPath);
				fNew = gNew + hNew;
				if (NodeDetails[Index + i].F == FLT_MAX || NodeDetails[Index + i].F > fNew) {
					Node node{};
					node.F = fNew;
					node.Position = succ[i];
					node.ParentNodeIndex = p.Index;
					node.Index = Index +i;
					OpenList.insert(node);


					NodeDetails[node.Index].F = fNew;
					NodeDetails[node.Index].H = hNew;
					NodeDetails[node.Index].G = gNew;
					NodeDetails[node.Index].Position = succ[i];

					NodeDetails[node.Index].ParentNodeIndex = p.Index;



				}

			}
		}
		Index += 7;

	}
	return nullptr;
}




CollisionSystem::CollisionSystem()
{
	m_PathGrid = new Node[50 * 50];

	m_BlockedNodeList = new bool[50 * 50];
	memset(m_BlockedNodeList, false, sizeof(bool) * 50 * 50);
}

void CollisionSystem::CheckCollisions()
{	
	//temp 

	static bool Init{ false };

	uint32_t Offset{};
	for (auto i = m_Colliders.begin(); i != m_Colliders.end(); i++) {
		i->second.SetStatus(false);
	}
	for (auto i = m_Colliders.begin(); i != std::next(m_Colliders.end(),-1) ; i++) {
			ColliderBackEnd* FirstCollider = &i->second;
			Float2 FirstMoveAmount = FirstCollider->GetMoveAmount();

			Offset++;
		for (auto j = std::next(m_Colliders.begin(),Offset); j != m_Colliders.end(); j++) {

			ColliderBackEnd* SecondCollider = &j->second;
			Float2 SecondMoveAmount = SecondCollider->GetMoveAmount();
			//x axis
			if (FirstCollider->GetPosition().x + FirstCollider->GetSize().x*2 > SecondCollider->GetPosition().x &&
				FirstCollider->GetPosition().x < SecondCollider->GetPosition().x + SecondCollider->GetSize().x*2&&
				FirstCollider->GetPosition().y +FirstCollider->GetSize().y*2>SecondCollider->GetPosition().y&&
				FirstCollider->GetPosition().y <SecondCollider->GetPosition().y + SecondCollider->GetSize().y*2) {


			
				FirstCollider->SetPosition({ FirstCollider->GetPosition().x - FirstMoveAmount.x ,FirstCollider->GetPosition().y-FirstMoveAmount.y });
				SecondCollider->SetPosition({ SecondCollider->GetPosition().x - SecondMoveAmount.x,SecondCollider->GetPosition().y - SecondMoveAmount.y});
			
				
				
					FirstCollider->SetStatus(true);
					SecondCollider->SetStatus(true);
			}
			
		}

	}
	for (auto i = m_Colliders.begin(); i != m_Colliders.end(); i++) {
		//uint32_t X = i->second.GetPosition().x/ TILESIZE;
		//uint32_t Y = i->second.GetPosition().y/ TILESIZE;
		//m_BlockedNodeList[(Y * 50) + X] = true;
		//m_PathGrid[(Y * 50) + X].NodeIndex;
	}


	
	//Reset the pathfinder
	memset(m_PathGrid, 0, sizeof(int) * 50 * 50);

}
bool CollisionSystem::IsBlocked(ColliderBackEnd* movingCollider, Float2 Position)
{
	for (auto it = m_Colliders.begin(); it != m_Colliders.end(); it++) {
		if (&it->second == movingCollider)
			continue;
		//CalcDistance
		float DistanceX= std::fabs(it->second.GetPosition().x - Position.x);
		float DistanceY = std::fabs(it->second.GetPosition().y - Position.y);
		if (DistanceX < movingCollider->GetSize().x + it->second.GetSize().x &&
			DistanceY < movingCollider->GetSize().y + it->second.GetSize().y)
			return true;
	}
	return false;
}
void CollisionFN1() {
	/*
	int FirstSignX, SecondSignX{};
	int FirstSignY, SecondSignY{};

	FirstSignX = int(*(int*)&FirstMoveAmount.x >> 31) & 1;
	SecondSignX = int(*(int*)&SecondMoveAmount.x >> 31) & 1;


	FirstSignY = int(*(int*)&FirstMoveAmount.y >> 31) & 1;
	SecondSignY = int(*(int*)&SecondMoveAmount.y >> 31) & 1;


	if (FirstSignX == SecondSignX) {
		if (std::fabs(FirstCollider->GetMoveAmount().x) > std::fabs(SecondCollider->GetMoveAmount().x)) {
			SecondCollider->SetPosition({ SecondCollider->GetPosition().x + (m_OnCollisionMovePercentage * (FirstCollider->GetMoveAmount().x + SecondCollider->GetMoveAmount().x)),SecondCollider->GetPosition().y });
			FirstCollider->SetPosition({ FirstCollider->GetPosition().x - FirstCollider->GetMoveAmount().x ,FirstCollider->GetPosition().y });
		}
		else if (std::fabs(SecondCollider->GetMoveAmount().x) > std::fabs(FirstCollider->GetMoveAmount().x)) {
			FirstCollider->SetPosition({ FirstCollider->GetPosition().x + (m_OnCollisionMovePercentage * (SecondCollider->GetMoveAmount().x + FirstCollider->GetMoveAmount().x)),FirstCollider->GetPosition().y });
			SecondCollider->SetPosition({ SecondCollider->GetPosition().x - SecondCollider->GetMoveAmount().x ,SecondCollider->GetPosition().y });
		}

	}
	else {
		if (std::fabs(FirstCollider->GetMoveAmount().x) > std::fabs(SecondCollider->GetMoveAmount().x)) {
			SecondCollider->SetPosition({ SecondCollider->GetPosition().x + (m_OnCollisionMovePercentage * (FirstCollider->GetMoveAmount().x - SecondCollider->GetMoveAmount().x)),SecondCollider->GetPosition().y });
			FirstCollider->SetPosition({ FirstCollider->GetPosition().x - FirstCollider->GetMoveAmount().x ,FirstCollider->GetPosition().y });
		}
		else if (std::fabs(SecondCollider->GetMoveAmount().x) > std::fabs(FirstCollider->GetMoveAmount().x)) {
			FirstCollider->SetPosition({ FirstCollider->GetPosition().x + (m_OnCollisionMovePercentage * (SecondCollider->GetMoveAmount().x - FirstCollider->GetMoveAmount().x)),FirstCollider->GetPosition().y });
			SecondCollider->SetPosition({ SecondCollider->GetPosition().x - SecondCollider->GetMoveAmount().x ,SecondCollider->GetPosition().y });
		}
		else {
			FirstCollider->SetPosition({ FirstCollider->GetPosition().x - FirstCollider->GetMoveAmount().x,FirstCollider->GetPosition().y });
			SecondCollider->SetPosition({ SecondCollider->GetPosition().x - SecondCollider->GetMoveAmount().x ,SecondCollider->GetPosition().y });
		}
	}
	//Y
	if (FirstSignY == SecondSignY) {
		if (std::fabs(FirstCollider->GetMoveAmount().y) > std::fabs(SecondCollider->GetMoveAmount().y)) {
			SecondCollider->SetPosition({ SecondCollider->GetPosition().x ,SecondCollider->GetPosition().y + (m_OnCollisionMovePercentage * (FirstCollider->GetMoveAmount().y - SecondCollider->GetMoveAmount().y)) });
			FirstCollider->SetPosition({ FirstCollider->GetPosition().x, FirstCollider->GetPosition().y - FirstCollider->GetMoveAmount().y });
		}
		else if (std::fabs(SecondCollider->GetMoveAmount().y) > std::fabs(FirstCollider->GetMoveAmount().y)) {
			FirstCollider->SetPosition({ SecondCollider->GetPosition().x,FirstCollider->GetPosition().y + (m_OnCollisionMovePercentage * (SecondCollider->GetMoveAmount().y - FirstCollider->GetMoveAmount().y)) });
			SecondCollider->SetPosition({ SecondCollider->GetPosition().x , SecondCollider->GetPosition().y - SecondCollider->GetMoveAmount().y });
		}

	}
	else {
		if (std::fabs(FirstCollider->GetMoveAmount().y) > std::fabs(SecondCollider->GetMoveAmount().y)) {
			SecondCollider->SetPosition({ SecondCollider->GetPosition().x ,SecondCollider->GetPosition().y + (m_OnCollisionMovePercentage * (FirstCollider->GetMoveAmount().y + SecondCollider->GetMoveAmount().y)) });
			FirstCollider->SetPosition({ FirstCollider->GetPosition().x, FirstCollider->GetPosition().y - FirstCollider->GetMoveAmount().y });
		}
		else if (std::fabs(SecondCollider->GetMoveAmount().y) > std::fabs(FirstCollider->GetMoveAmount().y)) {
			FirstCollider->SetPosition({ SecondCollider->GetPosition().x,FirstCollider->GetPosition().y + (m_OnCollisionMovePercentage * (SecondCollider->GetMoveAmount().y + FirstCollider->GetMoveAmount().y)) });
			SecondCollider->SetPosition({ SecondCollider->GetPosition().x , SecondCollider->GetPosition().y - SecondCollider->GetMoveAmount().y });
		}

	}
	*/
}





void ColliderBackEnd::Update(Float2* Position, Float2* Size)
{
	m_Position = Position;
	m_Size = Size;
}

void ColliderBackEnd::UpdateMoveAmount(Float2 MoveAmount)
{
	m_MoveAmount = MoveAmount;
}

