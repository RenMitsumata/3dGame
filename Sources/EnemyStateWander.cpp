#include "EnemyStateWander.h"
#include "Enemy.h"
#include "WayNode.h"
#include "WayNodeManager.h"
#include "Manager.h"

EnemyStateWander::EnemyStateWander()
{
}


EnemyStateWander::~EnemyStateWander()
{
	if (nodelist) {
		delete[] nodelist;
	}
}

void EnemyStateWander::Init(Enemy* enemy)
{
	velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	waitCnt = 0;
	desNodeNum = 0;
	nodeMgr = Manager::Get()->GetScene()->GetNodeManager();
	state = e_ARRIVE;
}

void EnemyStateWander::Update(Enemy* enemy)
{
	if (nodeMgr == nullptr) {
		nodeMgr = Manager::Get()->GetScene()->GetNodeManager();
		return;
	}


	switch (state) {
	case e_MOVE:
		// 位置更新して
		enemy->AddPos(velocity);

		// 目標ノードとの距離取得
		XMFLOAT3 ownerPos = enemy->GetPos();

		// 一定以下ならば、e_ARRIVEに移行
		XMFLOAT3 desPos = nodeMgr->GetNode(nodelist[desNodeNum])->GetPos();

		XMFLOAT3 restPos = desPos - ownerPos;
		XMVECTOR restVec = XMLoadFloat3(&restPos);

		float length;
		XMStoreFloat(&length, XMVector3Length(restVec));
		if (length < 1.0f) {
			state = e_ARRIVE;
		}
		break;

	case e_ARRIVE:
		// 行動の更新
		waitCnt++;
		if (waitCnt > COOLTIME) {
			waitCnt = 0;
			state = e_MOVE;
			if (dest == false) {
				desNodeNum++;
				if (desNodeNum == nodeNum - 1) {
					dest = true;
				}
			}
			else {
				desNodeNum--;
				if (desNodeNum == 0) {
					dest = false;
				}
			}
			bool isMinus = false;
			XMFLOAT3 distDir = nodeMgr->GetNode(nodelist[desNodeNum])->GetPos() - enemy->GetPos();
			if (distDir.x < 0.0f) {
				isMinus = true;
			}
			XMStoreFloat3(&distDir, XMVector3Normalize(XMLoadFloat3(&distDir)));
			XMFLOAT3 stdRot = XMFLOAT3(0.0f, 0.0f, 1.0f);
			float dotRes;
			XMVECTOR dotResVec = XMVector3Dot(XMLoadFloat3(&stdRot), XMLoadFloat3(&distDir));
			XMStoreFloat(&dotRes, dotResVec);
			float rotRad = acosf(dotRes);
			if (isMinus) {
				rotRad *= -1.0f;
			}
			enemy->SetRot(XMFLOAT3(0.0f, rotRad + XMConvertToRadians(180.0f), 0.0f));

			velocity = distDir * WALK_SPEED;
		}

		break;
	}
}

void EnemyStateWander::SetNode(unsigned short* nodeArray, unsigned short num)
{
	nodelist = new unsigned short[num];
	for (int i = 0; i < num; i++) {
		nodelist[i] = nodeArray[i];
	}
	nodeNum = num;
}
