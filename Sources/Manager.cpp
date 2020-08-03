/*===========================================
	DirectX11 ゲームプログラム
	Author: Ren Mitsumata
*********************************************

===========================================*/
#include "window.h"
#include "DXManager.h"
#include "Scene.h"
#include "Game.h"
#include "Title.h"
#include "InputManager.h"
#include "CollisionManager.h"
#include "AudioManager.h"
#include "EnemyManager.h"

#include "Manager.h"



Manager* Manager::instance;

Manager::Manager()
{
}


Manager::~Manager()
{
	Uninit();
	if (instance) {
		delete instance;
	}
}

void Manager::Init(HINSTANCE hInstance, int nCmdShow)
{
	window = new Window;
	window->Init(hInstance,nCmdShow);
	dxManager = new DXManager;
	dxManager->Init();
	input = new InputManager;
	input->Init();
	colManager = new CollisionManager;
	colManager->Init();
	audio = new AudioManager;
	audio->Init();
	enemyManager = new EnemyManager;
	enemyManager->Init();
	scene = new Title;
	scene->Init();
	
}

void Manager::Uninit()
{
	if (enemyManager) {
		enemyManager->Uninit();
		delete enemyManager;
		enemyManager = nullptr;
	}
	if (audio) {
		audio->Uninit();
		delete audio;
	}
	audio = nullptr;

	if (colManager) {
		colManager->Uninit();
		delete colManager;
	}
	colManager = nullptr;

	if (scene) {
		scene->Uninit();
		delete scene;
	}
	scene = nullptr;

	if (dxManager) {
		dxManager->Uninit();
		delete dxManager;
	}
	dxManager = nullptr;

	if (window) {
		window->Uninit();
		delete window;
	}
	window = nullptr;
}

void Manager::Update()
{
	input->Update();
	enemyManager->Update();
	if (input->GetKeyTrigger('T')) {
		dxManager->ToggleFrameMode();
	}
	if (input->GetKeyTrigger('E')) {
		isDebug = isDebug ^ true;
	}
	scene->Update();
}

void Manager::Draw()
{
	//dxManager->BeginDepth();
	//scene->Draw();	// W LV OrthoProj
	dxManager->BeginDeferred();
	scene->Draw(); // WVP , ↑↑の結果のテクスチャ , ↑↑で使ったfar
	dxManager->Begin();
	scene->DrawDeferred();


	enemyManager->Draw();
	dxManager->End();
}

void Manager::ChangeScene(Scene* nextScene)
{
	if (scene) {
		scene->Uninit();
		delete scene;
	}
	scene = nextScene;
	scene->Init();
}

// インスタンスゲッター
Manager* Manager::Get()
{
	if (!instance) {
		instance = new Manager();
	}
	return instance;
}

void Manager::Delete() {
	if (instance) {
		delete instance;
		instance = nullptr;
	}
}


// その他メンバゲッター
Window* Manager::GetWindow()
{
	return window;
}

DXManager* Manager::GetDXManager()
{
	return dxManager;
}

Scene* Manager::GetScene()
{
	return scene;
}

InputManager* Manager::GetInput()
{
	return input;
}

CollisionManager* Manager::GetColManager()
{
	return colManager;
}

AudioManager* Manager::GetAudio()
{
	return audio;
}

EnemyManager* Manager::GetEnemyManager()
{
	return enemyManager;
}




