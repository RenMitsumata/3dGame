#include "MapManager.h"
#include <io.h>
#include "Manager.h"
#include "Scene.h"
#include <string>
#include "Wall.h"
#include "WayNode.h"
#include "WayNodeManager.h"
#include "Enemy.h"
#include "Player.h"
#include "Floor.h"
#include "Goal.h"

MapManager::MapManager()
{
}


MapManager::~MapManager()
{
}

void MapManager::Init()
{
}

void MapManager::Uninit()
{
}

void MapManager::ReadMap(const char* filename, std::mutex& mut)
{
	Scene* scene = Manager::Get()->GetScene();
	WayNodeManager* nodeMgr = scene->GetNodeManager();

	// マップデータの書いてあるファイルを読み込む
	FILE* fp;
	long fileSize;
	fp = fopen(filename, "rt");
	fileSize = _filelength(_fileno(fp));
	
	// バッファに書き込む
	unsigned char* buffer = new unsigned char[fileSize];
	for (int i = 0; i < fileSize; i++) {
		buffer[i] = fgetc(fp);
	}

	// ファイルを閉じる
	fclose(fp);
	
	// 読み込んだデータを解析
	char currentData = buffer[0];
	unsigned int currentPoint = 0;

	// 開き中かっこまで読み飛ばす
	while (currentPoint < fileSize) {
		while (currentData != '{') {
			currentPoint++;
			currentData = buffer[currentPoint];
			if (currentPoint >= fileSize) {
				return;
			}
		}		

		while (currentData != ':') {
			currentPoint++;
			currentData = buffer[currentPoint];
		}
		// 識別子を判定
		char identity[24];
		unsigned char iNum = 0;
		while (currentData != ';') {
			currentPoint++;
			currentData = buffer[currentPoint];
			identity[iNum] = currentData;
			iNum++;
		}
		std::string idName(identity,iNum - 1);

		
		if (idName == "WALL") {
			// 生成に必要なメンバ
			XMFLOAT3 pos;
			XMFLOAT3 rot;
			XMFLOAT3 siz;
			float posMem[3];
			char locBuffer[64];
			unsigned char locCnt = 0;


			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}


			// ポジションを取得
			
			for (int i = 0; i < 3; i++) {
				
				currentPoint++;
				currentData = buffer[currentPoint];
				locCnt = 0;
				while (currentData != ',' && currentData != ';') {
					locBuffer[locCnt] = currentData;
					locCnt++;
					currentPoint++;
					currentData = buffer[currentPoint];
				}
				
				posMem[i] = atof(locBuffer);
				ZeroMemory(locBuffer, 64);
			}
			pos = { posMem[0],posMem[1],posMem[2] };

			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			// 回転を取得
			ZeroMemory(posMem, sizeof(float) * 3);
			for (int i = 0; i < 3; i++) {

				currentPoint++;
				currentData = buffer[currentPoint];
				locCnt = 0;

				while (currentData != ',' && currentData != ';') {
					locBuffer[locCnt] = currentData;
					locCnt++;
					currentPoint++;
					currentData = buffer[currentPoint];
					
				}
			
				posMem[i] = atof(locBuffer);
				ZeroMemory(locBuffer, 64);
			}
			rot = { XMConvertToRadians(posMem[0]),XMConvertToRadians(posMem[1]),XMConvertToRadians(posMem[2]) };

			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			// サイズを取得
			ZeroMemory(posMem, sizeof(float) * 3);
			for (int i = 0; i < 3; i++) {

				currentPoint++;
				currentData = buffer[currentPoint];
				locCnt = 0;

				while (currentData != ',' && currentData != ';') {
					locBuffer[locCnt] = currentData;
					locCnt++;
					currentPoint++;
					currentData = buffer[currentPoint];					
				}
				posMem[i] = atof(locBuffer);
				ZeroMemory(locBuffer, 64);
			}
			siz = { posMem[0] * 0.01f,posMem[1] * 0.01f,posMem[2] * 0.01f };

			mut.lock();
			Wall* wall = scene->AddGameObject<Wall>(e_LAYER_GAMEOBJECT);
			wall->SetPos(pos);
			wall->SetRot(rot);
			wall->SetSiz(siz);
			mut.unlock();

		} // if(WALL)
			
		if (idName == "NODE"){
			// 生成に必要なメンバ
			XMFLOAT3 pos;
			unsigned short id;
			unsigned short neighbor[16];
			float posMem[3];
			char locBuffer[64];
			unsigned char locCnt = 0;

			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}


			// ポジションを取得

			for (int i = 0; i < 3; i++) {

				currentPoint++;
				currentData = buffer[currentPoint];
				locCnt = 0;
				while (currentData != ',' && currentData != ';') {
					locBuffer[locCnt] = currentData;
					locCnt++;
					currentPoint++;
					currentData = buffer[currentPoint];
				}

				posMem[i] = atof(locBuffer);
				ZeroMemory(locBuffer, 64);
			}
			pos = { posMem[0],posMem[1],posMem[2] };

			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			// IDを取得
			currentPoint++;
			currentData = buffer[currentPoint];
			locCnt = 0;
			while (currentData != ';') {				
				locBuffer[locCnt] = currentData;
				locCnt++;
				currentPoint++;
				currentData = buffer[currentPoint];
			}
			id = atof(locBuffer);
			ZeroMemory(locBuffer, 64);
			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			// 隣のノード情報登録及び距離計算
			unsigned char neighborCnt = 1;
			locCnt = 0;
			currentPoint++;
			currentData = buffer[currentPoint];
			while (currentData != ';') {
				if (currentData == ',') {
					neighborCnt++;
				}
				locBuffer[locCnt] = currentData;
				locCnt++;
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			unsigned short* neighborArray = new unsigned short[neighborCnt];
			char* pBuf = locBuffer;
			char neighborID[6];
			unsigned char itr = 0;

			

			while (*pBuf != '\0') {
				unsigned char neighborIDPos = 0;
				ZeroMemory(neighborID, sizeof(char) * 6);
				while (*pBuf != ',' && neighborIDPos < 6) {
					neighborID[neighborIDPos] = *pBuf;
					neighborIDPos++;
					pBuf++;
				}
				neighborArray[itr] = atoi(neighborID);				
				pBuf++;
				itr++;
			}

			WayNode* node = scene->AddGameObject<WayNode>(e_LAYER_GAMEOBJECT);

			node->SetPos(pos);
			node->SetID(id);
			node->SetNeighbor(neighborArray, neighborCnt);
			nodeMgr->AddList(node);
			delete[] neighborArray;
		} //if(NODE)
			
		if (idName == "ENEMY") {
			// 生成に必要なメンバ
			XMFLOAT3 pos;
			unsigned short* nodeArray;
			float posMem[3];
			char locBuffer[64];
			unsigned char locCnt = 0;

			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}


			// ポジションを取得

			for (int i = 0; i < 3; i++) {

				currentPoint++;
				currentData = buffer[currentPoint];
				locCnt = 0;
				while (currentData != ',' && currentData != ';') {
					locBuffer[locCnt] = currentData;
					locCnt++;
					currentPoint++;
					currentData = buffer[currentPoint];
				}

				posMem[i] = atof(locBuffer);
				ZeroMemory(locBuffer, 64);
			}
			pos = { posMem[0],posMem[1],posMem[2] };

			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			// 隣のノード情報登録及び距離計算
			unsigned char nodeCnt = 1;
			locCnt = 0;
			currentPoint++;
			currentData = buffer[currentPoint];
			while (currentData != ';') {
				if (currentData == ',') {
					nodeCnt++;
				}
				locBuffer[locCnt] = currentData;
				locCnt++;
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			nodeArray = new unsigned short[nodeCnt];
			char* pBuf = locBuffer;
			char nodeID[6];
			unsigned char itr = 0;



			while (*pBuf != '\0') {
				unsigned char nodeIDPos = 0;
				ZeroMemory(nodeID, sizeof(char) * 6);
				while (*pBuf != ',' && nodeIDPos < 6) {
					nodeID[nodeIDPos] = *pBuf;
					nodeIDPos++;
					pBuf++;
				}
				nodeArray[itr] = atoi(nodeID);
				pBuf++;
				itr++;
			}


			mut.lock();
			Enemy* enemy = scene->AddGameObject<Enemy>(e_LAYER_GAMEOBJECT);
			enemy->SetPos(pos);
			enemy->SetWanderNode(nodeArray, nodeCnt);
			mut.unlock();

			delete[] nodeArray;



		}// if(ENEMY)

		if (idName == "PLAYER") {
			mut.lock();
			Player* player = scene->AddGameObject<Player>(e_LAYER_GAMEOBJECT);
			mut.unlock();
		}

		if (idName == "FLOOR") {
			// 生成に必要なメンバ
			XMFLOAT3 pos;
			float posMem[3];
			char locBuffer[64];
			// ノードに必要なメンバ
			unsigned short id;
			unsigned short neighbor[16];

			unsigned char locCnt = 0;

			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}


			// ポジションを取得

			for (int i = 0; i < 3; i++) {

				currentPoint++;
				currentData = buffer[currentPoint];
				locCnt = 0;
				while (currentData != ',' && currentData != ';') {
					locBuffer[locCnt] = currentData;
					locCnt++;
					currentPoint++;
					currentData = buffer[currentPoint];
				}

				posMem[i] = atof(locBuffer);
				ZeroMemory(locBuffer, 64);
			}
			pos = { posMem[0],posMem[1],posMem[2] };

			mut.lock();
			Floor* floor = scene->AddGameObject<Floor>(e_LAYER_BACKGROUND);
			floor->SetPos(pos);
			mut.unlock();

			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			// ノードを生成
			currentPoint++;
			currentData = buffer[currentPoint];
			locCnt = 0;
			while (currentData != ';') {
				locBuffer[locCnt] = currentData;
				locCnt++;
				currentPoint++;
				currentData = buffer[currentPoint];
			}
			id = atof(locBuffer);
			ZeroMemory(locBuffer, 64);
			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			// 隣のノード情報登録及び距離計算
			unsigned char neighborCnt = 1;
			locCnt = 0;
			currentPoint++;
			currentData = buffer[currentPoint];
			while (currentData != ';') {
				if (currentData == ',') {
					neighborCnt++;
				}
				locBuffer[locCnt] = currentData;
				locCnt++;
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			unsigned short* neighborArray = new unsigned short[neighborCnt];
			char* pBuf = locBuffer;
			char neighborID[6];
			unsigned char itr = 0;



			while (*pBuf != '\0') {
				unsigned char neighborIDPos = 0;
				ZeroMemory(neighborID, sizeof(char) * 6);
				while (*pBuf != ',' && neighborIDPos < 6) {
					neighborID[neighborIDPos] = *pBuf;
					neighborIDPos++;
					pBuf++;
				}
				neighborArray[itr] = atoi(neighborID);
				pBuf++;
				itr++;
			}

			mut.lock();
			WayNode* node = scene->AddGameObject<WayNode>(e_LAYER_GAMEOBJECT);

			node->SetPos(pos);
			node->SetID(id);
			node->SetNeighbor(neighborArray, neighborCnt);
			nodeMgr->AddList(node);
			delete[] neighborArray;

			mut.unlock();
		}

		if (idName == "GOAL") {

			XMFLOAT3 pos;
			unsigned short id;
			unsigned short neighbor[16];
			float posMem[3];
			char locBuffer[64];
			unsigned char locCnt = 0;

			// 次の:まで読み飛ばす
			while (currentData != ':') {
				currentPoint++;
				currentData = buffer[currentPoint];
			}

			// ポジションを取得

			for (int i = 0; i < 3; i++) {

				currentPoint++;
				currentData = buffer[currentPoint];
				locCnt = 0;
				while (currentData != ',' && currentData != ';') {
					locBuffer[locCnt] = currentData;
					locCnt++;
					currentPoint++;
					currentData = buffer[currentPoint];
				}

				posMem[i] = atof(locBuffer);
				ZeroMemory(locBuffer, 64);
			}
			pos = { posMem[0],posMem[1],posMem[2] };

			mut.lock();
			Goal* goal = scene->AddGameObject<Goal>(e_LAYER_GAMEOBJECT);
			goal->SetPos(pos);
			mut.unlock();
		}

	} // while(currentPoint < fileSize) 

	
}
