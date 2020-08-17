#define NOMINMAX
#include "Player.h"
#include "Model.h"

void Player::Init()
{
	Model* model = new Model();
	model->Load("Assets/Models/SambaDancing2.fbx");
	model->SetOwner(this);
	siz = XMFLOAT3(0.01f, 0.01f, 0.01f);
	componentsList.push_back(model);
}
