#include "Game.h"
#include "Player.h"
#include "Camera.h"

void Game::Init()
{
	Camera* camera = AddGameObject<Camera>(e_LAYER_CAMERA);
	camera->SetPos(XMFLOAT3(0.0f, 0.0f, -8.0f));
	
	AddGameObject<Player>(e_LAYER_GAMEOBJECT);
}
