
#define NOMINMAX

#include "CollisionSphere.h"
#include "Model.h"

#include "Goal.h"


Goal::Goal()
{

}


Goal::~Goal()
{

}

void Goal::Init()
{
	siz = { 0.01f,0.01f,0.01f };
	Model* model = ComponentFactory::CreateComponent<Model>();
	model->Load("Assets/Models/goal.fbx");
	model->SetOwner(this);
	componentsList.push_back(model);

	CollisionSphere* col = new CollisionSphere;
	col->Init();
	col->SetRadius(1.5f);
	col->SetTag(e_COLTYPE_GOAL);
	col->SetOwner(this);
	componentsList.push_back(col);

}