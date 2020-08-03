/*===========================================
	DirectX11 ゲームプログラム
	Author: Ren Mitsumata
*********************************************
　全てのマネージャを管理するクラス
===========================================*/
#ifndef MANAGER_H_
#define MANAGER_H_

/* -----（インクルードガード）----- */

#include <windows.h>


// 前方宣言
class Window;
class DXManager;
class Scene;
class InputManager;
class CollisionManager;
class AudioManager;
class EnemyManager;

class Manager
{
private:
	static Manager* instance;
	bool isDebug = true;
	// 以下、管理したいクラス
	Window* window;
	DXManager* dxManager;
	Scene* scene;
	InputManager* input;
	CollisionManager* colManager;
	AudioManager* audio;
	EnemyManager* enemyManager;
	// コンストラクタ、コピーコンストラクター、オペレータ＝は秘匿
	Manager();
	~Manager();
	Manager(const Manager& r){}
	Manager& operator= (const Manager& r){}
public:	
	void Init(HINSTANCE hInstance, int nCmdShow);
	void Uninit();
	void Update();
	void Draw();
	void ChangeScene(Scene* nextScene);
	bool GetDebug() { return isDebug; }

	static Manager* Get();
	static void Delete();
	
	Window* GetWindow();
	DXManager* GetDXManager();
	Scene* GetScene();
	InputManager* GetInput();
	CollisionManager* GetColManager();
	AudioManager* GetAudio();
	EnemyManager* GetEnemyManager();
};

/* -----（インクルードガード）----- */
#endif