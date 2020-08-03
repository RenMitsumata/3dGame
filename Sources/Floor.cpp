#define NOMINMAX
#include "Floor.h"
#include "Model.h"
#include "Manager.h"
#include "Scene.h"

Floor::Floor()
{
}


Floor::~Floor()
{
}

void Floor::Init()
{
	siz = { 0.01f,0.01f,0.01f };
	Model* model = ComponentFactory::CreateComponent<Model>();
	model->Load("Assets/Models/floor001.fbx");
	model->SetOwner(this);
	componentsList.push_back(model);
}