#pragma once
#include "DXManager.h"
#include "EnemyState.h"
#include <queue>

class EnemyStateAlert :	public EnemyState
{
	enum DetailStatus {
		e_CALCULATE = 0,	// ノード計算中
		e_RUSH				// 現場に移動中
	};

private:
	std::queue<unsigned short> nodeQue;	// 移動するノードのキュー（移動終わったノードは消していく）
	DetailStatus status = e_CALCULATE;
	unsigned short curNode;				// 最後に通過したノード
	XMFLOAT3 velocity;					// 現在の移動方向
public:
	EnemyStateAlert();
	~EnemyStateAlert();
	void Init(Enemy* enemy)override;
	void Update(Enemy* enemy)override;
	void SetNode(unsigned short* nodeArray, unsigned short num);
};

