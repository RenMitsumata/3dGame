#include "Collision.h"
#include "CollisionManager.h"
#include <iterator>
#include <algorithm>

CollisionManager::CollisionManager()
{
}


CollisionManager::~CollisionManager()
{
}

void CollisionManager::Init()
{
}

void CollisionManager::Uninit()
{
}

void CollisionManager::PreUpdate() 
{
	//collisionList.erase(std::remove_if(collisionList.begin(), collisionList.end(), [](Collision* col) {return col->GetOwner() == nullptr; }), collisionList.end());

	for (Collision* col : collisionList) {
		col->SetLastPos(col->GetOwner()->GetPos());
	}
}

void CollisionManager::Update()
{
	/*
		Œ»ÝA‚S•ª–Ø‚ðÌ—p‚¹‚¸‰¼‰^—p’†
	*/
	//collisionList.erase(std::remove_if(collisionList.begin(), collisionList.end(), [](Collision* col) {return col == nullptr; }), collisionList.end());

	if (collisionList.size() > 1) {
		std::list<Collision*>::iterator mine = collisionList.begin();
		std::list<Collision*>::iterator other = collisionList.begin();
		other++;
		while(mine != collisionList.end()) {
			Collision* myAdd = *mine;
			other = mine;
			other++;
			while(other != collisionList.end()) {
				Collision* otherAdd = *other;
				otherAdd->Dispatch(myAdd);
				other++;
			}
			mine++;
		}
	}
	

}

void CollisionManager::AddCollision(Collision* col)
{
	collisionList.push_back(col);
}

void CollisionManager::Delete(Collision * col)
{
	collisionList.erase(std::remove_if(collisionList.begin(), collisionList.end(), [col](Collision* collision) {return collision == col; }), collisionList.end());
}
