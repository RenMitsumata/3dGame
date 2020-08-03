#define NOMINMAX
#include "Bullet.h"
#include "Model.h"
#include "CollisionSphere.h"

Bullet::Bullet()
{
}


Bullet::~Bullet()
{
}

void Bullet::Init()
{
	pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	siz = XMFLOAT3(0.002f, 0.002f, 0.002f);
	front = XMFLOAT3(0.0f, 0.0f, 1.0f);
	Model* model = ComponentFactory::CreateComponent<Model>();
	model->Load("Assets/Models/ball.fbx");
	model->SetOwner(this);
	componentsList.push_back(model);
	CollisionSphere* col = new CollisionSphere;
	col->Init();
	col->SetRadius(1.0f);
	col->SetTag(e_COLTYPE_BULLET);
	col->SetOwner(this);
	componentsList.push_back(col);

}

void Bullet::Update() 
{
	pos += velocity;
}