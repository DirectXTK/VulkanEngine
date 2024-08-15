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
int ConvertPositionToNodeIndex(Float2 Position) {
	uint32_t X = (1 + Position.x) / TILESIZE;
	uint32_t Y = (1 - Position.y) / TILESIZE;
	return (Y * 50) + X;
}

void GenerateSuccesors(Node Parent,Node * succ) {
	int NodeX{ Parent.NodeIndex/50};
	int NodeY{Parent.NodeIndex-(NodeX*50)};
	//Left side
	succ[0].NodeIndex = {(NodeY*(50+1))+NodeX-1 };
	succ[0].ParentNodeIndex = { Parent.ParentNodeIndex};

	
	succ[1].NodeIndex = { (NodeY * (50 )) + NodeX - 1 };
	succ[1].ParentNodeIndex = { Parent.ParentNodeIndex };

	succ[2].NodeIndex = { (NodeY * (50 - 1)) + NodeX - 1 };
	succ[2].ParentNodeIndex = { Parent.ParentNodeIndex };


	//Middle
	succ[3].NodeIndex = { (NodeY * (50 + 1)) + NodeX  };
	succ[3].ParentNodeIndex = { Parent.ParentNodeIndex };


	succ[4].NodeIndex = { (NodeY * (50 - 1)) + NodeX - 1 };
	succ[4].ParentNodeIndex = { Parent.ParentNodeIndex };

	//Right side
	succ[5].NodeIndex = { (NodeY * (50 + 1)) + NodeX + 1 };
	succ[5].ParentNodeIndex = { Parent.ParentNodeIndex };

	succ[6].NodeIndex = { (NodeY * (50 )) + NodeX + 1 };
	succ[6].ParentNodeIndex = { Parent.ParentNodeIndex };

	succ[7].NodeIndex = { (NodeY * (50 - 1)) + NodeX + 1 };
	succ[7].ParentNodeIndex = { Parent.ParentNodeIndex };



	



}
uint32_t CalculateHValue(uint32_t CurrentNode, uint32_t Destination) {
	uint32_t CurrentNodeY = uint32_t(CurrentNode/50);
	uint32_t CurrentNodeX = CurrentNode - (50*CurrentNodeY);

	uint32_t DestinationY = uint32_t(Destination / 50);
	uint32_t DestinationX = Destination - (50 * DestinationY);
	return std::fabs(CurrentNodeY - DestinationY) + std::fabs(CurrentNodeX - DestinationX);
}
bool Collider::IsBlocked(int NodeIndex) {
	return m_System->IsBlocked(NodeIndex);
}
Float2 Collider::GetPathToObj(Float2 CurrentPos, Float2 FinalPath)
{
	int SourceIndex = ConvertPositionToNodeIndex(CurrentPos);
	int DestIndex = ConvertPositionToNodeIndex(FinalPath);
	Node* NodeDetails = new Node[50*50];
	Node Succ[8];
	std::set<Node> OpenList{};
	bool ClosedList[50 * 50];

	memset(ClosedList, 0, sizeof(bool) * 50 * 50);

	for (uint32_t i = 0; i < 50; i++) {

		for (uint32_t j = 0; j < 50; j++) {
			
			NodeDetails[j + (i * 50)].F = FLT_MAX;
			NodeDetails[j + (i * 50)].G = FLT_MAX;
			NodeDetails[j + (i * 50)].H = FLT_MAX;
			NodeDetails[j + (i * 50)].ParentNodeIndex= -1;
			NodeDetails[j + (i * 50)].NodeIndex = j + (i * 50);

		}



	}

	NodeDetails[SourceIndex].F = 0.0f;
	NodeDetails[SourceIndex].G = 0.0f;
	NodeDetails[SourceIndex].H = 0.0f;
	NodeDetails[SourceIndex].ParentNodeIndex = SourceIndex;
	NodeDetails->NodeIndex = SourceIndex;

	OpenList.insert(NodeDetails[SourceIndex]);

	bool FoundDest{false};

	while (!OpenList.empty()) {
		Node p = *OpenList.begin();
		
		OpenList.erase(OpenList.begin());

		ClosedList[p.NodeIndex] = true;

		float gNew, hNew, fNew;
		GenerateSuccesors(p, Succ);
		for (uint32_t i = 0; i < 8; i++) {
			if (Succ[i].NodeIndex == DestIndex) {
				Core::Log(ErrorType::Info, "Found the destination.",Succ[i].NodeIndex);
			}
			else if (ClosedList[Succ[i].NodeIndex] == false && IsBlocked(Succ[i].NodeIndex) == false) {
				gNew = NodeDetails[p.NodeIndex].G + 1.0f;
				hNew = CalculateHValue(Succ[i].NodeIndex, DestIndex);
				fNew = gNew + hNew;

				if (NodeDetails[Succ[i].NodeIndex].F == FLT_MAX || NodeDetails[Succ[i].NodeIndex].F > fNew) {
					OpenList.insert(Succ[i]);

					NodeDetails[Succ[i].NodeIndex].F = fNew;
					NodeDetails[Succ[i].NodeIndex].G = gNew;
					NodeDetails[Succ[i].NodeIndex].H = hNew;

					NodeDetails[Succ[i].NodeIndex].ParentNodeIndex = p.NodeIndex;

				}
			}
		}

	}



	delete[] NodeDetails;
	return Float2();
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
		uint32_t X = (1+i->second.GetPosition().x)/TILESIZE;
		uint32_t Y = (1-i->second.GetPosition().y)/ TILESIZE;
		m_BlockedNodeList[(Y * 50) + X] = true;
		//m_PathGrid[(Y * 50) + X].NodeIndex;
	}


	
	//Reset the pathfinder
	memset(m_PathGrid, 0, sizeof(int) * 50 * 50);

}
bool CollisionSystem::IsBlocked(int NodeIndex)
{
	return m_BlockedNodeList[NodeIndex];
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

