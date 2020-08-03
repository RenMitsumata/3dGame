#pragma once
#include "GameObject.h"

class EnemyState;
class EnemyManager;
//enum EnemyPhase;

class Enemy : public GameObject
{
private:
	EnemyState* state = nullptr;
	EnemyManager* manager;
	unsigned short* nodeArray;
	unsigned short nodeNum;
public:
	Enemy();
	~Enemy();
	void Init()override;
	void Update()override;
	void ColAction(Component* comp)override;
	void SetWanderNode(unsigned short* nodeArray, unsigned short num);
	EnemyState* GetState() { return state; }
	//void ChangeState(EnemyPhase phase);
};

