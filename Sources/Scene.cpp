
#include "Scene.h"
#include <algorithm>
#include "Camera.h"

Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::Init()
{
}

void Scene::Uninit()
{
	for (int i = 0; i < 5; i++) {
		for (GameObject* obj : objectList[i]) {
			obj->Uninit();
			delete obj;
		}
	}
}

void Scene::Update()
{
	colMgr.PreUpdate();
	for (int i = 0; i < 5; i++) {
		for (GameObject* obj : objectList[i]) {
			obj->Update();
		}
	}
	colMgr.Update();
	for (int i = 0; i < 5; i++) {
		
		objectList[i].erase(std::remove_if(objectList[i].begin(), objectList[i].end(), [](GameObject* obj) {return obj->IsDestroy(); }), objectList[i].end());

	}

}

void Scene::Draw()
{
	for (int i = 0; i < 5; i++) {
		for (GameObject* obj : objectList[i]) {
			obj->Draw();
		}
	}
}

void Scene::DrawDeferred()
{
}

void Scene::SetCameraMatrix(XMFLOAT4X4* p_ViewMatrix, XMFLOAT4X4* p_ProjectionMatrix)
{
	p_CameraViewMatrix = p_ViewMatrix;
	p_CameraProjectionMatrix = p_ProjectionMatrix;
}

void Scene::SetAnotherCamMatrix(XMFLOAT4X4 * p_ViewMatrix, XMFLOAT4X4 * p_ProjectionMatrix)
{
	p_AnotherCamViewMat = p_ViewMatrix;
	p_AnotherCamProjMat = p_ProjectionMatrix;
}
