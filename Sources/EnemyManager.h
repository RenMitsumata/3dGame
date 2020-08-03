#pragma once

#include <list>

class Enemy;
class UI;

enum EnemyPhase {
	e_ENEMYPHASE_NORMAL = 0,	// 通常フェーズ
	e_ENEMYPHASE_CAUTION,		// 警戒フェーズ
	e_ENEMYPHASE_EVASION,		// 捜索フェーズ
	e_ENEMYPHASE_ALERT			// 攻撃フェーズ
};




class EnemyManager
{
private:
	std::list<Enemy*> enemyList;
	UI* ui = nullptr;
public:
	EnemyManager();
	~EnemyManager();
	void Init();
	void Uninit();
	void Update();
	void Draw();
	void AddEnemy(Enemy* enemy);
	void DeleteEnemy(Enemy* enemy);
	void ChangePhase(EnemyPhase phase);
};

