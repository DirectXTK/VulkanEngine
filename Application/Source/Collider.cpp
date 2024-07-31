#include "Collider.h"

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




void CollisionSystem::CheckCollisions()
{	
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

