#define NOMINMAX
#include "Game.h"
#include "Manager.h"
#include "DXManager.h"
#include "GameObject.h"
#include "Player.h"
#include "Field.h"
#include "Camera.h"
#include "Wall.h"
#include "UI.h"
#include "Enemy.h"
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <mutex>
#include <thread>
#include "MapManager.h"
#include "WayNodeManager.h"
#include "Floor.h"
#include "Title.h"
#include "InputManager.h"
#include "DeferredObject.h"

InputManager* input;
void Loading(Game* game);
std::mutex _mut;

Game::Game()
{
}


Game::~Game()
{
}

void Game::Init()
{
	loadingPolygon = new UI;
	loadingPolygon->SetTextureAndSize("Assets/Textures/t_loading001.png");
	deferredPolygon = new UI[4];
	for (int i = 0; i < 4; i++) {
		deferredPolygon[i].SetTextureAndSize(nullptr, XMFLOAT3((1024.0f - 240.0f), 67.5f * (2 * i),0.0f), 240.0f, 135.0f);
	}
	albedoPolygon = new UI;
	albedoPolygon->SetTextureAndSize(nullptr , XMFLOAT3(0.0f,0.0f,0.5f));

	equip = new UI*[2];
	for (int i = 0; i < 2; i++) {
		equip[i] = new UI;
	}
	equip[0]->SetTextureAndSize("Assets/Textures/gunA.png", XMFLOAT3(50.0f, 400.0f, 0.3f), 240.0f, 160.0f);
	equip[1]->SetTextureAndSize("Assets/Textures/knife.png", XMFLOAT3(50.0f, 400.0f, 0.3f), 240.0f, 160.0f);


	input = Manager::Get()->GetInput();
	srand(time(NULL));
	dxManager = Manager::Get()->GetDXManager();
	context = dxManager->GetDeviceContext();

	nodeMgr = new WayNodeManager;
	mapMgr = new MapManager;

	//Loading();


	auto func = [this]{Loading(this); };
	std::thread loadingThread(func);
	loadingThread.detach();

	
}

void Game::Uninit()
{
	
	for (int i = 0; i < 5; i++) {
		for (GameObject* obj : objectList[i]) {
			obj->Uninit();
			delete obj;
		}
	}
	if (mapMgr) {
		mapMgr->Uninit();
		delete mapMgr;
	}
	if (nodeMgr) {
		nodeMgr->Uninit();
		delete nodeMgr;
	}
	if (albedoPolygon) {
		albedoPolygon->Uninit();
		delete albedoPolygon;
	}
	if (deferredPolygon) {
		deferredPolygon->Uninit();
		delete[] deferredPolygon;
	}

	if (loadingPolygon) {
		loadingPolygon->Uninit();
		delete loadingPolygon;
	}

	if (equip[0]) {
		for (int i = 0; i < 2; i++) {
			equip[i]->Uninit();
			delete equip[i];
		}
		delete[] equip;
	}

}

void Game::Update()
{
	if (phase == e_CLEARMOVIE || phase == e_CLEAR) {

	}
	if (input->GetKeyTrigger(VK_BACK)) {
		Manager::Get()->ChangeScene(new Title);
	}
	if (phase == e_LOADING) {
		_mut.lock();
		loadingPolygon->ChangeColor();
		_mut.unlock();
		return;
	}

	if (input->GetPadLTTrigger()) {
		phase = e_CHOOSEITEM;
	}

	if (input->GetPadLTRelease()) {
		phase = e_SCENE;
	}

	if (phase == e_CHOOSEITEM) {
		if (input->GetPadTrigger(XINPUT_GAMEPAD_DPAD_UP)) {
			curEquip++;
		}
		if (input->GetPadTrigger(XINPUT_GAMEPAD_DPAD_DOWN)) {
			curEquip--;
		}

		if (curEquip > 1) {
			curEquip %= 2;
		}
		return;
	}


	colMgr.PreUpdate();

	for (int i = 0; i < 5; i++) {
		for (GameObject* obj : objectList[i]) {
			obj->Update();
		}
	}
	colMgr.Update();
	for (int i = 0; i < 5; i++) {

		objectList[i].erase(std::remove_if(objectList[i].begin(), objectList[i].end(), [](GameObject* obj) 
		{
			bool isDelete = obj->IsDestroy(); 
			if (isDelete) {
				obj->Uninit();
				delete obj;
				obj = nullptr;
				return true;
			}
			else return false;
		}),
			objectList[i].end());

	}

	


	if (phase != e_LOADING && loadingPolygon) {
		loadingPolygon->Uninit();
		delete loadingPolygon;
		loadingPolygon = nullptr;
	}
}

void Game::Draw()
{
	if (phase != e_LOADING) {
		for (int i = 0; i < 5; i++) {
			for (GameObject* obj : objectList[i]) {
				obj->Draw();
			}
		}		
	}
	
	if (phase == e_LOADING && loadingPolygon) {
		//_mut.lock();
		loadingPolygon->Draw();
		//_mut.unlock();
	}

	if (phase == e_CHOOSEITEM) {
		equip[curEquip]->Draw();
	}
}

void Game::DrawDeferred()
{
	ID3D11ShaderResourceView* srv[1];
	srv[0] = dxManager->GetSRV(0);
	if (srv[0] == nullptr) {
		return;
	}
	albedoPolygon->SetDeferredTexture(srv[0]);
	albedoPolygon->Draw();

	//if (phase != e_LOADING) {
		/*for (int i = 0; i < 4; i++) {
			ID3D11ShaderResourceView* srv[1];
			srv[0] = dxManager->GetSRV(i);
			deferredPolygon[i].SetDeferredTexture(srv[0]);
			deferredPolygon[i].Draw();
		}*/
	//}
	
		
	
}

void Game::SetPhase(Phase phase)
{
	this->phase = phase;
}

void Game::SetMapManager(MapManager * mapMgr)
{
	this->mapMgr = mapMgr;
}

void Loading(Game* game)
{
	MapManager* mapMgr = new MapManager;
	mapMgr->Init();
	//Camera* camera = AddGameObject<Camera>(e_LAYER_CAMERA);
	mapMgr->ReadMap("Assets/Maps/stage001.txt",_mut);

	//Field* field = AddGameObject<Field>(e_LAYER_BACKGROUND);
	//Floor* floor = AddGameObject<Floor>(e_LAYER_BACKGROUND);

	//Enemy* enemy = AddGameObject<Enemy>(e_LAYER_GAMEOBJECT);
	//enemy->SetPos(XMFLOAT3(5.0f, 0.0f, 5.0f));
	//UI* ui = AddGameObject<UI>(e_LAYER_UI);
	//ui->LoadTexture("Assets/Textures/t_title001.png");
	Sleep(1000);
	game->SetMapManager(mapMgr);
	game->SetPhase(Game::e_SCENE);
}
