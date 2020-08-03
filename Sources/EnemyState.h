#pragma once

class Enemy;

class EnemyState
{
public:
	EnemyState();
	virtual ~EnemyState();
	virtual void Init(Enemy* enemy) = 0;
	virtual void Update(Enemy* enemy) = 0;
};

