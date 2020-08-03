#pragma once

#define WALK_SPEED (0.1f)
#define COOLTIME (200)

class WayNodeManager;

#include "DXManager.h"

#include "EnemyState.h"
class EnemyStateWander : public EnemyState
{
private:
	enum DetailState {
		e_MOVE,		// 目的地へ移動中
		e_ARRIVE	// 目的地へ到着(次の場所を検索)
	};
	DetailState state = e_ARRIVE;
	XMFLOAT3 velocity;
	WayNodeManager* nodeMgr = nullptr;
	unsigned short desNodeNum;
	bool dest = false;	// ノード配列をどっち方向でたどるか
	unsigned short waitCnt;	// 移動後、その場で止まっている時間
	unsigned short nodeNum = 0;
	unsigned short* nodelist = nullptr;
public:
	EnemyStateWander();
	~EnemyStateWander();
	void Init(Enemy* enemy)override;
	void Update(Enemy* enemy)override;
	void SetNode(unsigned short* nodeArray, unsigned short num);
};

