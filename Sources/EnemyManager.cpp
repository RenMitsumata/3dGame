#include "EnemyManager.h"
#include "Enemy.h"
#include "UI.h"

EnemyManager::EnemyManager()
{
}


EnemyManager::~EnemyManager()
{
	if (ui) {
		ui->Uninit();
		delete ui;
	}
}

void EnemyManager::Init()
{
}

void EnemyManager::Uninit()
{
}

void EnemyManager::Update()
{
	// フェーズタイマーを更新（例えば、一定時間経過で警戒フェーズに移行）
}

void EnemyManager::Draw()
{
	// フェーズ状態を描画(UI)
	if (ui) {
		ui->Draw();
	}
}

void EnemyManager::AddEnemy(Enemy* enemy)
{
	enemyList.push_back(enemy);
}

void EnemyManager::DeleteEnemy(Enemy* enemy)
{
	//enemyList.remove(enemy);
}

void EnemyManager::ChangePhase(EnemyPhase phase)
{
	switch (phase) {
		case e_ENEMYPHASE_ALERT:
		{
			if (ui) {
				delete ui;
			}
			ui = new UI;
			ui->SetTextureAndSize("Assets/Textures/alert.png", XMFLOAT3(WINDOW_WIDTH / 2 - 50.0f, 10.0f, 0.0f), 100.0f, 50.0f);
			// 全てのエネミーを警戒状態に
			break;
		}
		case e_ENEMYPHASE_CAUTION:
		{
			if (ui) {
				delete ui;
			}
			ui = new UI;
			ui->SetTextureAndSize("Assets/Textures/caution.png", XMFLOAT3(WINDOW_WIDTH / 2 - 50.0f, 10.0f, 0.0f), 100.0f, 50.0f);
			// 全てのエネミーを注意状態に
			break;
		}
	}
}
