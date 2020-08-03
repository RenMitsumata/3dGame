#include "EnemyStateAlert.h"



EnemyStateAlert::EnemyStateAlert()
{
}


EnemyStateAlert::~EnemyStateAlert()
{
}

void EnemyStateAlert::Init(Enemy* enemy)
{
}

void EnemyStateAlert::Update(Enemy* enemy)
{
	if (status == e_CALCULATE) {
		// 移動しない
	}
	if (status == e_RUSH) {
		// 目標ノードへ急行
	}
}

void EnemyStateAlert::SetNode(unsigned short * nodeArray, unsigned short num)
{
}
